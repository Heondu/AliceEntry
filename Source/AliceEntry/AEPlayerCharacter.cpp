// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerCharacter.h"
#include "AEPlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
//케이블 컴포넌트는 플러그인이라 모듈에 추가해야 함
#include "CableComponent.h"
#include "DrawDebugHelpers.h"
#include "AEGameMode.h"

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
	GrapplingTiming = 0.2f;

	bIsAttacking = false;
	AttackEndComboState();

	DeadTimer = 0.5f;

	bCanMove = true;
	WalkSpeed = 200;
	SprintSpeed = 600;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprint = false;

	EState = ECharacterState::Stopped;

	RotationRate = 0.1f;

	SwingTime = 1.5f;
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
		EState = ECharacterState::Stopped;
		});
	AnimInstance->OnSlideEnd.AddLambda([this]() -> void {
		bCanMove = true;
		EState = ECharacterState::Stopped;
		});
	AnimInstance->OnFlyingModeStart.AddLambda([this]() -> void {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		});
	AnimInstance->OnFlyingModeEnd.AddLambda([this]() -> void {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		});

	RopeVisibility(false);

	OriginMaterials = GetMesh()->GetMaterials();
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

	if (ShouldSlide())
	{
		Slide();
	}

	if (EState == ECharacterState::Walking || EState == ECharacterState::Running)
	{
		if (LastMovementInputVector == FVector::ZeroVector)
		{
			EState = ECharacterState::Stopped;
		}

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	if (bSmoothRotate)
	{
		FRotator Rotation = FMath::RInterpTo(GetActorRotation(), TargetRotator, DeltaTime, 20);
		SetActorRotation(Rotation);
	}

	if (bIsDissolve)
	{
		TagPercent = FMath::FInterpTo(TagPercent, 1, DeltaTime, 2);

		for (int i = 0; i < MaterialInstances.Num(); i++)
		{
			MaterialInstances[i]->SetScalarParameterValue(TEXT("Appearance"), TagPercent);
			GetMesh()->SetMaterial(i, MaterialInstances[i]);
		}
	}

	if (bIsAppearance)
	{
		TagPercent = FMath::FInterpTo(TagPercent, 0, DeltaTime, 2);

		for (int i = 0; i < MaterialInstances.Num(); i++)
		{
			MaterialInstances[i]->SetScalarParameterValue(TEXT("Appearance"), TagPercent);
			GetMesh()->SetMaterial(i, MaterialInstances[i]);
		}
	}

	if (bIsSprint)
	{
		if (LastMovementInputVector.X < 0) GetCharacterMovement()->MaxWalkSpeed = 400;
		else GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
	PlayerInputComponent->BindAction("Grapple", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::AttachGrapplingHook);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::DetachGrapplingHookWhenSwing);
	PlayerInputComponent->BindAction("Roll", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Roll);
	PlayerInputComponent->BindAction("SprintOrSlide", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("SprintOrSlide", EInputEvent::IE_Released, this, &AAEPlayerCharacter::ChangeFromSprintToWalk);
	PlayerInputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AAEPlayerCharacter::Attack);
}

void AAEPlayerCharacter::MoveForward(float AxisValue)
{
	LastMovementInputVector.X = AxisValue;

	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (AxisValue == 0.0f) return;

	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0, Rot.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, AxisValue);
	EState = bIsSprint ? ECharacterState::Running : ECharacterState::Walking;
}

void AAEPlayerCharacter::MoveRight(float AxisValue)
{
	LastMovementInputVector.Y = AxisValue;

	if (!bCanMove) return;
	if (bInGrapplingAnimation) return;
	if (AxisValue == 0.0f) return;

	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0, Rot.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, AxisValue);
	EState = bIsSprint ? ECharacterState::Running : ECharacterState::Walking;
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

void AAEPlayerCharacter::AttackCheck()
{
	FVector Direction = Camera->GetForwardVector();
	Direction.Z = 0;
	Direction.GetSafeNormal();
	FVector End = GetActorLocation() + Direction * AttackRange;

	LookAtCamera();
	TargetRotator = FRotationMatrix::MakeFromX(End - GetActorLocation()).Rotator();

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

//#if ENABLE_DRAW_DEBUG
//	FVector TraceVec = Direction * AttackRange;
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
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
		if (HitResult.Actor.IsValid() && HitResult.GetActor()->ActorHasTag("Enemy"))
		{
			LOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(Damage, DamageEvent, GetController(), this);
		}
	}
}

void AAEPlayerCharacter::Roll()
{
	if (!bCanMove) return;
	if (GetCharacterMovement()->IsFalling()) return;

	bCanMove = false;
	bIsAttacking = false;
	AttackEndComboState();
	SetCanBeDamaged(false);

	if (EState == ECharacterState::Stopped)
	{
		AnimInstance->PlayRollAnim("Backward");
		return;
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetActorRotation(FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator());

	if (EState == ECharacterState::Walking)
	{
		AnimInstance->PlayRollAnim("Forward");
	}
	else if (EState == ECharacterState::Running)
	{
		AnimInstance->PlayRollAnim("Sprint");
	}

	EState = ECharacterState::Rolling;
}

void AAEPlayerCharacter::OnAttackMontageEnded()
{
	//CHECK(bIsAttacking);
	//CHECK(CurrentCombo > 0);
	bIsAttacking = false;
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
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());

	FVector HitDirection = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
	GetCharacterMovement()->AddImpulse(HitDirection * 800, true);

	if (Health <= 0.0f)
	{
		bIsDead = true;
		bCanMove = false;
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
		EState = ECharacterState::Dead;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		GetWorldTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			Cast<AAEGameMode>(GetWorld()->GetAuthGameMode())->ShowGameOver();
			}), DeadTimer, false);
	}
	else if (CanBeDamaged() && EState != ECharacterState::Grappling && EState != ECharacterState::Swing && bIsAttacking)
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

void AAEPlayerCharacter::LookAtCamera()
{
	bSmoothRotate = true;
	FTimerHandle Timer = {};
	GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]() -> void {
		bSmoothRotate = false;
		}), RotationRate, false);
}

void AAEPlayerCharacter::Appearance(float Time)
{
	OriginMaterials = GetMesh()->GetMaterials();

	for (int i = 0; i < TagMaterials.Num(); i++)
	{
		MaterialInstances.Add(UMaterialInstanceDynamic::Create(TagMaterials[i], this));
	}

	TagPercent = 1;
	bIsAppearance = true;

	FTimerHandle Timer = {};
	GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]() -> void {
		bIsAppearance = false;
		for (int i = 0; i < OriginMaterials.Num(); i++)
		{
			GetMesh()->SetMaterial(i, OriginMaterials[i]);
		}
		}), Time, false);
}

void AAEPlayerCharacter::Dissolve(float Time)
{
	for (int i = 0; i < TagMaterials.Num(); i++)
	{
		MaterialInstances.Add(UMaterialInstanceDynamic::Create(TagMaterials[i], this));
	}
	
	TArray<AActor*> Actors;
	GetAttachedActors(Actors);
	for (AActor* Actor : Actors)
		Actor->Destroy();
	SetActorEnableCollision(false);
	TagPercent = 0;
	bIsDissolve = true;
	bCanMove = false;
	
	FTimerHandle Timer = {};
	GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([this]() -> void {
		Destroy();
		}), Time, false);
}

void AAEPlayerCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprint = true;
}

void AAEPlayerCharacter::ChangeFromSprintToWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprint = false;
}

void AAEPlayerCharacter::Slide()
{
	bCanMove = false;
	bIsAttacking = false;
	AttackEndComboState();

	AnimInstance->PlaySlideAnim();

	EState = ECharacterState::Sliding;
}

bool AAEPlayerCharacter::ShouldSlide()
{
	if (EState != ECharacterState::Running) return false;
	if (!bCanMove) return false;
	if (bInGrapplingAnimation) return false;
	if (!GetCharacterMovement()->IsMovingOnGround()) return false;
	if (GetCharacterMovement()->IsFalling()) return false;
	if (GetCharacterMovement()->Velocity.Size() <= 0) return false;

	int Angle = GetGroundAngle();

	if (Angle > -25) return false;

	bool bResult;
	if (PreviousAngle != Angle) bResult = false;
	else bResult = true;
	PreviousAngle = Angle;
	return bResult;
}

float AAEPlayerCharacter::GetGroundAngle()
{
	int Angle = FMath::RoundToInt(FRotationMatrix::MakeFromX(GetActorLocation() - PreviousLocation).Rotator().Pitch);
	PreviousLocation = GetActorLocation();

	return Angle;
}

void AAEPlayerCharacter::Grapple()
{
	if (!bCanMove) return;
	if (!IsValid(GrapplePointRef)) return;
	if (!GrapplePointRef->bCanGrapple) return;
	if (bGrapplingHookAttached)
	{
		DetachGrapplingHook();
	}

	float Distance = (GetActorLocation() - GrapplePointRef->GetActorLocation()).Size();
	if (Distance <= GrappleThrowDistance && CurrentGrapplePoint != GrapplePointRef)
	{
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
			AnimInstance->PlayGrappleAnim(true);
		}
		else
		{
			AnimInstance->PlayGrappleAnim(false);
		}
		
		EState = ECharacterState::Grappling;
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

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Camera->GetComponentLocation(), DetectedActor->GetActorLocation(), ECollisionChannel::ECC_Visibility))
	{
		if (HitResult.GetActor() == DetectedActor)
		{
			if (DetectedActor != GrapplePointRef)
			{
				DeactivateGrapplePoint();
				GrapplePointRef = Cast<AAEGrapplePoint>(DetectedActor);
				GrapplePointRef->Activate(this, true);
			}
		}
		else
		{
			LOG_S(Warning);
			DeactivateGrapplePoint();
			GrapplePointRef = Cast<AAEGrapplePoint>(DetectedActor);
			GrapplePointRef->Activate(this, false);
		}
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
	float LerpValue = (AnimInstance->GetCurrentActiveMontage() == AnimInstance->GrappleGround ? GroundSpeedCurve : AirSpeedCurve)->GetFloatValue(MontagePosition);
	float HeightValue = (AnimInstance->GetCurrentActiveMontage() == AnimInstance->GrappleGround ? GroundHeightOffsetCurve : AirHeightOffsetCurve)->GetFloatValue(MontagePosition);
	FVector NewLocation = UKismetMathLibrary::VLerp(StartingPosition, GrapplingDestination, LerpValue) + FVector(0.0f, 0.0f, HeightValue);
	SetActorLocation(NewLocation);
}

void AAEPlayerCharacter::StartGrapplingMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
	StartingPosition = GetActorLocation();
	bMovingWithGrapple = true;
}

void AAEPlayerCharacter::ResetMovement()
{
	bMovingWithGrapple = false;
	CurrentGrapplePoint = NULL;
	GetCharacterMovement()->GravityScale = 1.0f;
	bInGrapplingAnimation = false;
	EState = ECharacterState::Stopped;
}

void AAEPlayerCharacter::RopeVisibility(bool bVisible)
{
	Rope->SetVisibility(bVisible);
	Hook->SetVisibility(bVisible);
}

void AAEPlayerCharacter::MoveRope()
{
	MontagePosition = AnimInstance->Montage_GetPosition(AnimInstance->GetCurrentActiveMontage());
	Rope->CableLength = (AnimInstance->GetCurrentActiveMontage() == AnimInstance->GrappleGround ? GroundRopeLength : AirRopeLength)->GetFloatValue(MontagePosition) * RopeBaseLength;
	float Value = (AnimInstance->GetCurrentActiveMontage() == AnimInstance->GrappleGround ? GroundRopePosition : AirRopePosition)->GetFloatValue(MontagePosition);
	FVector NewLocation = UKismetMathLibrary::VLerp(GetMesh()->GetSocketLocation(FName("Hand_LSocket")), CurrentGrapplePoint->GetActorLocation(), Value);
	Hook->SetWorldLocation(NewLocation);
}

void AAEPlayerCharacter::Swing()
{
	if (!bCanMove) return;

	Hook->SetWorldLocation(HookPoint);

	if (!GetCharacterMovement()->IsFalling())
	{
		LastSwingDirection = GetActorForwardVector();
		return;
	}

	FVector Distance = GetActorLocation() - HookPoint;
	float Dot = FVector::DotProduct(Distance, GetVelocity());
	FVector Force = Distance.GetSafeNormal() * Dot * -2.0f;
	GetCharacterMovement()->AddForce(Force);
	GetCharacterMovement()->AddForce(GetActorForwardVector() * 100000);

	FVector DirectionNormal = (HookPoint - GetActorLocation()).GetSafeNormal();
	FVector VelocityNormal = GetVelocity().GetSafeNormal();
	FVector Cross = FVector::CrossProduct(VelocityNormal, DirectionNormal) * -1.0f;
	SwingAngle = FRotationMatrix::MakeFromZX(DirectionNormal, Cross).Rotator().Roll;

	EState = ECharacterState::Swing;
}

void AAEPlayerCharacter::AttachGrapplingHook()
{
	if (!bCanMove) return;
	if (!IsValid(GrapplePointRef)) return;
	float Distance = (GetActorLocation() - GrapplePointRef->GetActorLocation()).Size();
	if (Distance > GrappleThrowDistance) return;

	if (GrapplePointRef->Tags[0] == "Grapple")
	{
		Grapple();
	}
	else if (GrapplePointRef->Tags[0] == "Swing")
	{
		if (CurrentGrapplePoint == GrapplePointRef)
		{
			DetachGrapplingHook();
			return;
		}

		AttachedTime = UGameplayStatics::GetTimeSeconds(GetWorld());
		bGrapplingHookAttached = true;
		HookPoint = GrapplePointRef->GetActorLocation();
		RopeVisibility(true);
		CurrentGrapplePoint = GrapplePointRef;

		FVector Location;
		FRotator Rotation;
		GetController()->GetPlayerViewPoint(Location, Rotation);
		SetActorRotation(FRotator(0, Rotation.Yaw, 0));

		LastSwingDirection = GetActorForwardVector();
	}
}

void AAEPlayerCharacter::DetachGrapplingHook()
{
	CurrentGrapplePoint = NULL;
	bGrapplingHookAttached = false;
	RopeVisibility(false);
}

void AAEPlayerCharacter::DetachGrapplingHookWhenSwing()
{
	if (!GetCharacterMovement()->IsFalling()) return;

	DetachGrapplingHook();
}