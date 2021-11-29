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
	AttackRadius = 50.0f;
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
		bIsAttacking = true;
	}
}

void AAEBunnyCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 1.0f;
	
	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid() && HitResult.GetActor()->ActorHasTag("Enemy"))
		{
			LOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(Damage, DamageEvent, GetController(), this);
		}
	}
}

void AAEBunnyCharacter::AttackMove()
{
	GetCharacterMovement()->AddImpulse(GetActorForwardVector() * AttackMoveForce, true);
	CamShake();
}
