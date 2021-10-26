// Fill out your copyright notice in the Description page of Project Settings.


#include "AECharacter.h"
#include "AEAnimInstance.h"

AAECharacter::AAECharacter()
{
	BoomArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	BoomArm->SetupAttachment(RootComponent);
	BoomArm->TargetArmLength = 300.0f;
	BoomArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(BoomArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	TurnRate = 45;
	LookRate = 45;

	AttackRange = 200.0f;
	AttackRadius = 50.0f;
}

void AAECharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAECharacter::AttackCheck);
}

void AAECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AAECharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAECharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAECharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAECharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AAECharacter::Attack);
}

void AAECharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && IsAttacking == false)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator Yaw(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAECharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && IsAttacking == false)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRot(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AAECharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * GetWorld()->GetDeltaSeconds() * TurnRate);
}

void AAECharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * GetWorld()->GetDeltaSeconds() * LookRate);
}
