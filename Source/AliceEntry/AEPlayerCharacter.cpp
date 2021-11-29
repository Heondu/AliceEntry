// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerCharacter.h"
#include "AEPlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
//케이블 컴포넌트는 플러그인이라 모듈에 추가해야 함
#include "CableComponent.h"

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

	Rope = CreateDefaultSubobject<UCableComponent>(TEXT("Rope"));
	Rope->SetupAttachment(GetMesh(), FName("Hand_LSocket"));
	Rope->NumSegments = 10;
	Rope->SolverIterations = 10;
	Rope->CableWidth = 2;
	Rope->NumSides = 16;

	Hook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook"));
	Hook->SetupAttachment(RootComponent);
	Hook->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	Hook->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	Rope->SetAttachEndTo(NULL, FName("Hook"));
	Rope->EndLocation = FVector::ZeroVector;

	bUseControllerRotationPitch = false;
	//카메라 회전에 따라 캐릭터 회전
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	TurnRate = 45;
	LookRate = 45;

	DetectionRadius = 2000;
	GrappleThrowDistance = 1200;

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

	RopeVisibility(false);
}

void AAEPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAEPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForGrapplePoints();

	if (bInGrapplingAnimation)
	{
		MoveRope();

		if (bMovingWithGrapple)
		{
			GrapplingMovement();
		}
	}

	if (bGrapplingHookAttached)
	{
		Swing();
	}
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
	PlayerInputComponent->BindAction("Grapple", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Grapple);
	PlayerInputComponent->BindAction("Attach Grappling Hook", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::AttachGrapplingHook);
	PlayerInputComponent->BindAction("Detach Grappling Hook", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::DetachGrapplingHook);
	PlayerInputComponent->BindAction("Roll", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Roll);
	PlayerInputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Slide);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Attack);
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

	AnimInstance->PlayRollAnim(false);
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

	FVector HitDirection = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
	GetCharacterMovement()->AddImpulse(HitDirection * 800, true);

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

void AAEPlayerCharacter::Slide()
{
	if (!bCanMove) return;
	if (GetCharacterMovement()->IsFalling()) return;
	if (bInGrapplingAnimation) return;

	bCanMove = false;
	bIsAttacking = false;
	AttackEndComboState();

	//AnimInstance->PlaySlideAnim();
}

void AAEPlayerCharacter::Grapple()
{
	if (!bCanMove) return;
	if (!IsValid(GrapplePointRef)) return;
	if (bGrapplingHookAttached)
	{
		DetachGrapplingHook();
	}

	float Distance = (GetActorLocation() - GrapplePointRef->GetActorLocation()).Size();
	if (Distance <= GrappleThrowDistance && CurrentGrapplePoint != GrapplePointRef)
	{
		if (bMovingWithGrapple)
		{
			FVector LaunchVelocity = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), GrapplingDestination) * 200.0f;
			LaunchCharacter(LaunchVelocity, false, false);
		}

		bInGrapplingAnimation = true;
		bMovingWithGrapple = false;
		CurrentGrapplePoint = GrapplePointRef;
		FVector TargetLocation = Cast<USceneComponent>(CurrentGrapplePoint->GetDefaultSubobjectByName(TEXT("Detection")))->GetComponentLocation();
		GrapplingDestination = TargetLocation + FVector(0.0f, 0.0f, 110.0f);

		FRotator NewRotation = FRotator::ZeroRotator;
		NewRotation.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GrapplingDestination).Yaw;
		SetActorRotation(NewRotation);

		RopeBaseLength = (GetActorLocation() - GrapplingDestination).Size();
		CurrentGrapplePoint->Use();

		if (GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->GravityScale = 1.0f;
			AnimInstance->Montage_Play(GrappleAir);
		}
		else
		{
			AnimInstance->Montage_Play(GrappleGround);
		}

	}
}

void AAEPlayerCharacter::CheckForGrapplePoints()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery7);
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;
	bool bResult;
	bResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		DetectionRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResults,
		true);

	if (bResult)
	{
		float HighestDotProduct = 0.7f;

		for (FHitResult Hit : HitResults)
		{
			FVector Unit = (Hit.Actor->GetActorLocation() - Camera->GetComponentLocation()).GetSafeNormal();
			float Dot = FVector::DotProduct(Camera->GetForwardVector(), Unit);

			if (Dot > HighestDotProduct)
			{
				DetectedActor = Hit.GetActor();
				HighestDotProduct = Dot;
			}
			else
			{
				DeactivateGrapplePoint();
			}
		}

		ActivateGrapplePoint();
	}
	else
	{
		DeactivateGrapplePoint();
	}
}

void AAEPlayerCharacter::ActivateGrapplePoint()
{
	if (!IsValid(DetectedActor)) return;

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Camera->GetComponentLocation(),
		DetectedActor->GetActorLocation(),
		ETraceTypeQuery::TraceTypeQuery6,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true);

	if (HitResult.GetActor() == DetectedActor)
	{
		if (DetectedActor != GrapplePointRef)
		{
			DeactivateGrapplePoint();
			GrapplePointRef = Cast<AAEGrapplePoint>(DetectedActor);
			GrapplePointRef->Activate(this);
		}
	}
	else
	{
		DeactivateGrapplePoint();
	}
}

void AAEPlayerCharacter::DeactivateGrapplePoint()
{
	if (!IsValid(GrapplePointRef)) return;

	GrapplePointRef->Deactivate();
	GrapplePointRef = NULL;
}

void AAEPlayerCharacter::GrapplingMovement()
{
	MontagePosition = AnimInstance->Montage_GetPosition(AnimInstance->GetCurrentActiveMontage());
	float LerpValue = (AnimInstance->GetCurrentActiveMontage() == GrappleGround ? GroundSpeedCurve : AirSpeedCurve)->GetFloatValue(MontagePosition);
	float HeightValue = (AnimInstance->GetCurrentActiveMontage() == GrappleGround ? GroundHeightOffsetCurve : AirHeightOffsetCurve)->GetFloatValue(MontagePosition);
	FVector NewLocation = UKismetMathLibrary::VLerp(StartingPosition, GrapplingDestination, LerpValue) + FVector(0.0f, 0.0f, HeightValue);
	SetActorLocation(NewLocation);
}

void AAEPlayerCharacter::StartGrapplingMovement()
{
	//GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->StopMovementImmediately();
	StartingPosition = GetActorLocation();
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	bMovingWithGrapple = true;
}

void AAEPlayerCharacter::ResetMovement()
{
	bMovingWithGrapple = false;
	CurrentGrapplePoint = NULL;
	GetCharacterMovement()->GravityScale = 1.0f;
	bInGrapplingAnimation = false;
	LOG(Warning, TEXT("A"));
}

void AAEPlayerCharacter::RopeVisibility(bool bVisible)
{
	Rope->SetVisibility(bVisible);
	Hook->SetVisibility(bVisible);
}

void AAEPlayerCharacter::MoveRope()
{
	MontagePosition = AnimInstance->Montage_GetPosition(AnimInstance->GetCurrentActiveMontage());
	Rope->CableLength = (AnimInstance->GetCurrentActiveMontage() == GrappleGround ? GroundRopeLength : AirRopeLength)->GetFloatValue(MontagePosition) * RopeBaseLength;
	float Value = (AnimInstance->GetCurrentActiveMontage() == GrappleGround ? GroundRopePosition : AirRopePosition)->GetFloatValue(MontagePosition);
	FVector NewLocation = UKismetMathLibrary::VLerp(GetMesh()->GetSocketLocation(FName("Hand_LSocket")), CurrentGrapplePoint->GetActorLocation(), Value);
	Hook->SetWorldLocation(NewLocation);
}

void AAEPlayerCharacter::Swing()
{
	Hook->SetWorldLocation(HookPoint);

	if (!GetCharacterMovement()->IsFalling()) return;

	FVector Distance = GetActorLocation() - HookPoint;
	float Dot = FVector::DotProduct(Distance, GetVelocity());
	FVector Force = Distance.GetSafeNormal() * Dot * -2.0f;
	GetCharacterMovement()->AddForce(Force);
	GetCharacterMovement()->AddForce(GetActorForwardVector() * 100000);
}

void AAEPlayerCharacter::AttachGrapplingHook()
{
	if (!IsValid(GrapplePointRef)) return;

	bGrapplingHookAttached = true;
	HookPoint = GrapplePointRef->GetActorLocation();
	RopeVisibility(true);
}

void AAEPlayerCharacter::DetachGrapplingHook()
{
	bGrapplingHookAttached = false;
	RopeVisibility(false);
}