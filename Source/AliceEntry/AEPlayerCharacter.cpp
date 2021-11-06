// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerCharacter.h"
#include "AEAnimInstance.h"
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

	//RopeComp = Cast<UCableComponent>(GetDefaultSubobjectByName(TEXT("Rope")));
	//HookComp = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Hook")));

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

	IsAttacking = false;
	AttackEndComboState();

	DeadTimer = 5.0f;

	CanMove = true;
	IsSliding = false;

	EState = ECharacterState::Stopped;
}


void AAEPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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
		CanMove = true; 
		SetCanBeDamaged(true);
		//bUseControllerRotationYaw = true;
		});
	AnimInstance->OnSlideEnd.AddLambda([this]() -> void {
		CanMove = true;
		IsSliding = false;
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

	if (ShouldSlide())
	{
		if (GetGroundAngle() < -25.0f)
		{
			Slide();
		}
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
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Grapple", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Grapple);
	PlayerInputComponent->BindAction("Roll", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Roll);
	PlayerInputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Slide);
}

void AAEPlayerCharacter::MoveForward(float AxisValue)
{
	if (!CanMove) return;
	if (bInGrapplingAnimation) return;

	if (AxisValue == 0.0f) return;
	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0, Rot.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, AxisValue);

	//AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AAEPlayerCharacter::MoveRight(float AxisValue)
{
	if (!CanMove) return;
	if (bInGrapplingAnimation) return;

	if (AxisValue == 0.0f) return;
	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0, Rot.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, AxisValue);

	//AddMovementInput(GetActorRightVector() * AxisValue);
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
	if (!CanMove) return;
	if (bInGrapplingAnimation) return;
	
	ACharacter::Jump();
}

float AAEPlayerCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void AAEPlayerCharacter::Grapple()
{
	if (!CanMove) return;

	if (!IsValid(GrapplePointRef)) return;

	float Distance = (GetActorLocation() - GrapplePointRef->GetActorLocation()).Size();
	if (Distance <= GrappleThrowDistance && CurrentGrapplePoint != GrapplePointRef)
	{
		if (bMovingWithGrapple)
		{
			FVector LaunchVelocity = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), GrapplingDestination) * 200.0f;
			LaunchCharacter(LaunchVelocity, false, false);
		}
		
		CanMove = false;
		bInGrapplingAnimation = true;
		bMovingWithGrapple = false;
		CurrentGrapplePoint = GrapplePointRef;
		FVector TargetLocation = Cast<USceneComponent>(CurrentGrapplePoint->GetDefaultSubobjectByName(TEXT("LandingZone")))->GetComponentLocation();
		//FVector Force = (FVector(TargetLocation.X, TargetLocation.Y, 0) - FVector(GetActorLocation().X, GetActorLocation().Y, 0)).GetSafeNormal() * 1000.0f;
		GrapplingDestination = TargetLocation + FVector(0.0f, 0.0f, 110.0f);// +Force;

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

void AAEPlayerCharacter::Attack()
{
}

void AAEPlayerCharacter::Roll()
{
	if (!CanMove) return;
	if (GetCharacterMovement()->IsFalling()) return;

	//bUseControllerRotationYaw = false;
	//SetActorRotation(FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator());
	CanMove = false;
	IsAttacking = false;
	SetCanBeDamaged(false);

	if (GetLastMovementInputVector() != FVector::ZeroVector)
	{
		AnimInstance->PlayRollAnim(false);
	}
	else
	{
		AnimInstance->PlayRollAnim(true);
	}
}

void AAEPlayerCharacter::Slide()
{
	if (!CanMove) return;
	if (GetCharacterMovement()->IsFalling()) return;

	CanMove = false;
	IsAttacking = false;
	IsSliding = true;

	AnimInstance->PlaySlideAnim();
}

bool AAEPlayerCharacter::ShouldSlide()
{
	if (GetCharacterMovement()->IsFalling()) return false;
	if (GetCharacterMovement()->Velocity.Size() <= 0) return false;

	return true;
}

float AAEPlayerCharacter::GetGroundAngle()
{
	float Angle = FRotationMatrix::MakeFromX(GetActorLocation() - PreviousLocation).Rotator().Pitch;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Angle : %f"), Angle));
	PreviousLocation = GetActorLocation();

	return Angle;
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
			FVector Unit = UKismetMathLibrary::GetDirectionUnitVector(Camera->GetComponentLocation(), Hit.Actor->GetActorLocation());
			float Dot = UKismetMathLibrary::Dot_VectorVector(Camera->GetForwardVector(), Unit);
			
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
		ETraceTypeQuery::TraceTypeQuery1,
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
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->StopMovementImmediately();
	StartingPosition = GetActorLocation();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	bMovingWithGrapple = true;
}

void AAEPlayerCharacter::ResetMovement()
{
	bMovingWithGrapple = false;
	CurrentGrapplePoint = NULL;
	GetCharacterMovement()->GravityScale = 1.0f;
	bInGrapplingAnimation = false;
	CanMove = true;
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

void AAEPlayerCharacter::OnAttackMontageEnded()
{
	CHECK(IsAttacking);
	CHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	//OnAttackEnd.Broadcast();
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

	if (Health <= 0.0f)
	{
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	else if (CanBeDamaged())
	{
		CanMove = false;
		IsAttacking = false;
		AttackEndComboState();
		AnimInstance->PlayHitAnim();
	}
	return DamageApplied;
}
