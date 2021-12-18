// Fill out your copyright notice in the Description page of Project Settings.


#include "AEBunnyCharacter.h"
#include "AEPlayerAnimInstance.h"
#include "DrawDebugHelpers.h"

AAEBunnyCharacter::AAEBunnyCharacter()
{

}

void AAEBunnyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAEBunnyCharacter::AttackCheck);
	AnimInstance->OnAttackMove.AddUObject(this, &AAEBunnyCharacter::AttackMove);

	AttackRange = 200.0f;
	AttackRadius = 150.0f;
	AttackMoveForce = 800.0f;
	MaxCombo = 3;
}

void AAEBunnyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAEBunnyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAEBunnyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAEBunnyCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAEBunnyCharacter::Jump);
	PlayerInputComponent->BindAction("Skill1", EInputEvent::IE_Pressed, this, &AAEBunnyCharacter::Skill1);
	PlayerInputComponent->BindAction("Skill2", EInputEvent::IE_Pressed, this, &AAEBunnyCharacter::Skill2);
	PlayerInputComponent->BindAction("Skill3", EInputEvent::IE_Pressed, this, &AAEBunnyCharacter::Skill3);
}

void AAEBunnyCharacter::MoveForward(float AxisValue)
{
	if (bIsAttacking) return;


	AAEPlayerCharacter::MoveForward(AxisValue);
}

void AAEBunnyCharacter::MoveRight(float AxisValue)
{
	if (bIsAttacking) return;

	AAEPlayerCharacter::MoveRight(AxisValue);
}

void AAEBunnyCharacter::Jump()
{
	if (bIsAttacking) return;

	AAEPlayerCharacter::Jump();
}

void AAEBunnyCharacter::Attack()
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
		bIsAttacking = true;
	}
}

void AAEBunnyCharacter::AttackCheck()
{
	FVector Direction = Camera->GetForwardVector();
	Direction.Z = 0;
	Direction.GetSafeNormal();
	FVector End = GetActorLocation() + Direction * AttackRange;

	LookAtCamera();
	TargetRotator = FRotationMatrix::MakeFromX(End - GetActorLocation()).Rotator();

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation() + GetActorForwardVector() * 100,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	//#if ENABLE_DRAW_DEBUG
	//	FVector TraceVec = Direction * AttackRange;
	//	FVector Center = GetActorLocation() + GetActorForwardVector() * 100 + TraceVec * 0.5f;
	//	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	//	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	//	float DebugLifeTime = 1.0f;
	//
	//	DrawDebugCapsule(
	//		GetWorld(),
	//		Center,
	//		HalfHeight,
	//		AttackRadius,
	//		CapsuleRot,
	//		DrawColor,
	//		false,
	//		DebugLifeTime
	//	);
	//#endif

	if (bResult)
	{
		LOG(Warning, TEXT("%d"), HitResults.Num());
		for (FHitResult HitResult : HitResults)
		{
			if (!HitResult.Actor.IsValid())
			{
				continue;
			}
			if (HitResult.GetActor()->ActorHasTag("Enemy"))
			{
				FDamageEvent DamageEvent;
				HitResult.Actor->TakeDamage(Damage, DamageEvent, GetController(), this);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, HitResult.Actor->GetActorLocation(), HitResult.Actor->GetActorRotation(), FVector(1), true, true, ENCPoolMethod::AutoRelease, true);
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitEffectSound, HitResult.Actor->GetActorLocation());
			}
			else if (HitResult.GetActor()->ActorHasTag("PhysicsObject"))
			{
				if (HitResult.GetActor()->IsRootComponentMovable()) {
					UStaticMeshComponent* MeshRootComp = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());
					MeshRootComp->AddForce(Camera->GetForwardVector() * 100000 * MeshRootComp->GetMass());
				}
			}
		}
	}
}

void AAEBunnyCharacter::AttackMove()
{
	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveForce, true);
	CamShake();
}

void AAEBunnyCharacter::Skill1()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	CurrentCombo = 0;
	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("1");
}

void AAEBunnyCharacter::Skill2()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	CurrentCombo = 0;
	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("2");
}

void AAEBunnyCharacter::Skill3()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	CurrentCombo = 0;
	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("3");
}