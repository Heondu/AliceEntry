// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerCharacter.h"
#include "AEAnimInstance.h"

AAEPlayerCharacter::AAEPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BoomArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	BoomArm->SetupAttachment(RootComponent);
	BoomArm->TargetArmLength = 300.0f;
	BoomArm->bUsePawnControlRotation = true;
	BoomArm->SetRelativeLocation(FVector(0.0f, 20.0f, 50.0f));
	BoomArm->SocketOffset = FVector(0.0f, 70.0f, 0.0f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(BoomArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	//카메라 회전에 따라 캐릭터 회전
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	TurnRate = 45;
	LookRate = 45;
}

void AAEPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AAEPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAEPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAEPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAEPlayerCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AAEBasicCharacter::Attack);
}

void AAEPlayerCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AAEPlayerCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AAEPlayerCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * GetWorld()->GetDeltaSeconds() * TurnRate);
}

void AAEPlayerCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * GetWorld()->GetDeltaSeconds() * LookRate);
}

float AAEPlayerCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}
