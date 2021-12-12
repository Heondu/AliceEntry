// Fill out your copyright notice in the Description page of Project Settings.


#include "AEBunnyCharacter.h"
#include "AEPlayerAnimInstance.h"

AAEBunnyCharacter::AAEBunnyCharacter()
{
	AttackRange = 200.0f;
	AttackRadius = 100.0f;
	AttackMoveForce = 800.0f;
	MaxCombo = 3;
}

void AAEBunnyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAEBunnyCharacter::AttackCheck);
	AnimInstance->OnAttackMove.AddUObject(this, &AAEBunnyCharacter::AttackMove);
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

	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("1");
}

void AAEBunnyCharacter::Skill2()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("2");
}

void AAEBunnyCharacter::Skill3()
{
	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (bIsAttacking) return;

	bIsAttacking = true;
	AnimInstance->PlaySkillAnim("3");
}