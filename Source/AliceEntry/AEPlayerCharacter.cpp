// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerCharacter.h"
#include "AEPlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

AAEPlayerCharacter::AAEPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.0f, 20.0f, 50.0f));
	SpringArm->SocketOffset = FVector(0.0f, 70.0f, 0.0f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	//카메라 회전에 따라 캐릭터 회전
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	TurnRate = 45;
	LookRate = 45;

	bIsAttacking = false;
	AttackEndComboState();

	DeadTimer = 5.0f;

	bCanMove = true;

	EState = ECharacterState::Stopped;
}


void AAEPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UAEPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != AnimInstance);
	AnimInstance->OnHitEnd.AddLambda([this]() -> void {
		bCanMove = true;
		});
	AnimInstance->OnAttackEnd.AddUObject(this, &AAEPlayerCharacter::OnAttackMontageEnded);
	AnimInstance->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			AnimInstance->JumpToAttackMontageSection(CurrentCombo);
		}
		});
	AnimInstance->OnRollEnd.AddLambda([this]() -> void {
		bCanMove = true;
		SetCanBeDamaged(true);
		});
}

void AAEPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAEPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAEBasicCharacter::Jump);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Roll", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Roll);
}

void AAEPlayerCharacter::MoveForward(float AxisValue)
{
	LastMovementInputVector.X = AxisValue;

	if (!bCanMove) return;
	if (AxisValue == 0.0f) return;

	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0, Rot.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, AxisValue);
}

void AAEPlayerCharacter::MoveRight(float AxisValue)
{
	LastMovementInputVector.Y = AxisValue;

	if (!bCanMove) return;
	if (AxisValue == 0.0f) return;

	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0, Rot.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, AxisValue);
}

void AAEPlayerCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * GetWorld()->GetDeltaSeconds() * TurnRate);
}

void AAEPlayerCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * GetWorld()->GetDeltaSeconds() * LookRate);
}

void AAEPlayerCharacter::Jump()
{
	if (!bCanMove) return;
	
	ACharacter::Jump();
}

float AAEPlayerCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void AAEPlayerCharacter::Attack()
{
}

void AAEPlayerCharacter::Roll()
{
	if (!bCanMove) return;
	if (bIsAttacking) return;
	if (GetCharacterMovement()->IsFalling()) return;

	bCanMove = false;
	bIsAttacking = false;
	AttackEndComboState();
	SetCanBeDamaged(false);

	if (LastMovementInputVector != FVector::ZeroVector)
	{
		AnimInstance->PlayRollAnim(false);
	}
	else
	{
		AnimInstance->PlayRollAnim(true);
	}
}

void AAEPlayerCharacter::OnAttackMontageEnded()
{
	CHECK(bIsAttacking);
	CHECK(CurrentCombo > 0);
	bIsAttacking = false;
	LOG_S(Warning);
	AttackEndComboState();
}

void AAEPlayerCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AAEPlayerCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

float AAEPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;
	LOG(Warning, TEXT("Health left %f"), Health);
	GetCharacterMovement()->AddImpulse(-GetActorForwardVector() * 800, true);

	if (Health <= 0.0f)
	{
		bIsDead = true;
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	else if (CanBeDamaged())
	{
		bCanMove = false;
		bIsAttacking = false;
		AttackEndComboState();
		AnimInstance->PlayHitAnim();
	}
	return DamageApplied;
}

void AAEPlayerCharacter::CamShake()
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), ShakeAttack, UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation(), 0.0f, 1000.0f, 1.0f, false);
}