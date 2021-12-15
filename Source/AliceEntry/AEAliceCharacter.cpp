// Fill out your copyright notice in the Description page of Project Settings.


#include "AEAliceCharacter.h"
#include "AEGun.h"
#include "AEPlayerAnimInstance.h"

AAEAliceCharacter::AAEAliceCharacter()
{
	AttackRange = 200.0f;
	AttackRadius = 50.0f;
	MaxCombo = 2;
}

void AAEAliceCharacter::BeginPlay()
{
	Super::BeginPlay();

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAEAliceCharacter::AttackCheck);
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

	AnimInstance->OnLeftShot.AddUObject(GunL, &AAEGun::PullTrigger);
	AnimInstance->OnRightShot.AddUObject(GunR, &AAEGun::PullTrigger);
}

void AAEAliceCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAEAliceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Skill1", EInputEvent::IE_Pressed, this, &AAEAliceCharacter::Skill1);
	PlayerInputComponent->BindAction("Skill2", EInputEvent::IE_Pressed, this, &AAEAliceCharacter::Skill2);
}

void AAEAliceCharacter::Attack()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;

	if (bIsAttacking)
	{
		if (!FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo)) return;
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
		bIsAttacking = true;
	}
}

void AAEAliceCharacter::Shoot()
{
	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);
	FVector End = Location + Rotation.Vector() * MaxRange;

	FVector Direction = Camera->GetForwardVector();
	Direction.Z = 0;
	Direction.GetSafeNormal();
	FVector TargetEnd = GetActorLocation() + Direction * MaxRange;

	LookAtCamera();
	TargetRotator = FRotationMatrix::MakeFromX(TargetEnd - GetActorLocation()).Rotator();

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
		if (nullptr != HitActor && HitActor->ActorHasTag("Enemy"))
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetController(), this);
		}
	}
}

void AAEAliceCharacter::Skill1()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	CurrentCombo = 0;
	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("1");
}

void AAEAliceCharacter::Skill2()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	CurrentCombo = 0;
	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("2");
}