// Fill out your copyright notice in the Description page of Project Settings.


#include "AEBunnyCharacter.h"
#include "AEAnimInstance.h"

AAEBunnyCharacter::AAEBunnyCharacter()
{

}

void AAEBunnyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	MaxCombo = 3;
}

void AAEBunnyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAEBunnyCharacter::AttackCheck);
}

void AAEBunnyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAEBunnyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAEBunnyCharacter::MoveRight);
}

void AAEBunnyCharacter::MoveForward(float AxisValue)
{
	if (IsAttacking) return;

	AAEPlayerCharacter::MoveForward(AxisValue);
}

void AAEBunnyCharacter::MoveRight(float AxisValue)
{
	if (IsAttacking) return;

	AAEPlayerCharacter::MoveRight(AxisValue);
}