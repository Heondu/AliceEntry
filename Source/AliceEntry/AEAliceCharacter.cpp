// Fill out your copyright notice in the Description page of Project Settings.


#include "AEAliceCharacter.h"
#include "AEGun.h"
#include "AEAnimInstance.h"

AAEAliceCharacter::AAEAliceCharacter()
{
	MaxCombo = 2;
}

void AAEAliceCharacter::BeginPlay()
{
	Super::BeginPlay();

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnNextAttackCheck.AddLambda([this]() -> void {
		if (IsComboInputOn)
		{
			Shoot();
		}
		});

	GunL = GetWorld()->SpawnActor<AAEGun>(GunClass);
	GunR = GetWorld()->SpawnActor<AAEGun>(GunClass);
	GunL->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Hand_lSocket"));
	GunR->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Hand_rSocket"));
	GunL->SetOwner(this);
	GunR->SetOwner(this);
}

void AAEAliceCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAEAliceCharacter::Attack()
{
	if (!CanMove) return;

	if (IsAttacking)
	{
		CHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		CHECK(CurrentCombo == 0);
		AttackStartComboState();
		AnimInstance->PlayAttackMontage();
		AnimInstance->JumpToAttackMontageSection(CurrentCombo);
		Shoot();
		IsAttacking = true;
	}
}

void AAEAliceCharacter::Shoot()
{
	GunL->PullTrigger();
	GunR->PullTrigger();

	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);

	FVector End = Location + Rotation.Vector() * MaxRange;

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	//ECC_EngineTraceChannel과 ECC_GameTraceChannel 구분 주의
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, CollisionParams);
	if (bSuccess)
	{
		FVector ShotDirection = -Rotation.Vector();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunL->ImpactEffect, Hit.Location, ShotDirection.Rotation());
		AActor* HitActor = Hit.GetActor();
		if (nullptr != HitActor)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetController(), this);
		}
	}
}
