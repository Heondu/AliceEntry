// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEBasicCharacter.h"
#include "AEGrapplePoint.h"
#include "GameFramework/Character.h"
#include "AEPlayerCharacter.generated.h"

UENUM()
enum class ECharacterState
{
	Stopped		UMETA(DisplayName == "Stopped"),
	Walking		UMETA(DisplayName == "Walking"),
	Running		UMETA(DisplayName == "Running"),
	Attacking	UMETA(DisplayName == "Attacking"),
	Grappling	UMETA(DisplayName == "Grappling"),
	Swing		UMETA(DisplayName == "Swing"),
	Rolling		UMETA(DisplayName == "Rolling"),
	Sliding		UMETA(DisplayName == "Sliding"),
	Dead		UMETA(DisplayName == "Dead"),
};

UCLASS()
class ALICEENTRY_API AAEPlayerCharacter : public AAEBasicCharacter
{
	GENERATED_BODY()

public:
	AAEPlayerCharacter();

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Grapple)
	class UCableComponent* Rope;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Grapple)
	class UStaticMeshComponent* Hook;

	void CheckForGrapplePoints();
	void ActivateGrapplePoint();
	void DeactivateGrapplePoint();
	void GrapplingMovement();
	void StartGrapplingMovement();
	void ResetMovement();
	void RopeVisibility(bool bVisible);
	void MoveRope();

	UFUNCTION()
	void OnAttackMontageEnded();

	void AttackStartComboState();
	void AttackEndComboState();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Appearance(float Time);
	void Dissolve(float Time);

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ECharacterState> EState;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void MoveForward(float AxisValue);
	virtual void MoveRight(float AxisValue);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Jump();
	void Grapple();
	void Swing();
	void AttachGrapplingHook();
	void DetachGrapplingHook();
	void DetachGrapplingHookWhenSwing();
	virtual void Attack();
	void Roll();
	void Sprint();
	void Slide();
	bool ShouldSlide();
	float GetGroundAngle();
	void ChangeFromSprintToWalk();
	
	void CamShake();
	void LookAtCamera();

	UPROPERTY()
	class UAEPlayerAnimInstance* AnimInstance;

	float TurnRate;
	float LookRate;

	UPROPERTY()
	FVector LastMovementInputVector;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, Meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ShakeAttack;

public:
	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	float DetectionRadius;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	float GrappleThrowDistance;

protected:
	UPROPERTY()
	AActor* DetectedActor;

	UPROPERTY()
	class AAEGrapplePoint* GrapplePointRef;

	UPROPERTY()
	class AAEGrapplePoint* CurrentGrapplePoint;

	UPROPERTY()
	FVector GrapplingDestination;

	UPROPERTY()
	FVector StartingPosition;

	UPROPERTY()
	bool bMovingWithGrapple;

	UPROPERTY()
	bool bInGrapplingAnimation;

	UPROPERTY()
	float MontagePosition;

	UPROPERTY()
	float RopeBaseLength;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* AirSpeedCurve;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* GroundSpeedCurve;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* AirHeightOffsetCurve;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* GroundHeightOffsetCurve;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* AirRopeLength;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* GroundRopeLength;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* AirRopePosition;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	UCurveFloat* GroundRopePosition;

	UPROPERTY(BlueprintReadOnly, Category = Grapple)
	bool bGrapplingHookAttached;

	UPROPERTY()
	FVector HookPoint;

	UPROPERTY()
	float AttachedTime;

	UPROPERTY()
	float GrapplingTiming;

	UPROPERTY(BlueprintReadOnly, Category = Grapple)
	float SwingAngle;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float WalkSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* HitSound;

	UPROPERTY()
	int PreviousAngle;

	UPROPERTY()
	FVector PreviousLocation;

	UPROPERTY()
	bool bIsSprint;

	UPROPERTY()
	bool bSmoothRotate;

	UPROPERTY()
	float RotationRate;

	UPROPERTY()
	FRotator TargetRotator;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginMaterials;

	UPROPERTY(EditDefaultsOnly, Category = Tag)
	TArray<UMaterialInterface*> TagMaterials;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MaterialInstances;

	UPROPERTY()
	bool bIsDissolve;

	UPROPERTY()
	bool bIsAppearance;

	UPROPERTY()
	float TagPercent;

	UPROPERTY()
	FTimerHandle SwingTimer;

	UPROPERTY()
	float SwingTime;

	UPROPERTY()
	FVector LastSwingDirection;
};
