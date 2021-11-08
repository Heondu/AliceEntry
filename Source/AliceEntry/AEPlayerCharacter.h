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
	Stopped	UMETA(DisplayName == "Stopped"),
	Moving		UMETA(DisplayName == "Moving"),
	Attacking	UMETA(DisplayName == "Attacking"),
	Grappling	UMETA(DisplayName == "Grappling"),
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
	virtual void Attack();
	void Roll();
	void Slide();
	bool ShouldSlide();
	float GetGroundAngle();

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ECharacterState> EState;

	float TurnRate;
	float LookRate;
	UPROPERTY()
	FVector PreviousLocation = FVector::ZeroVector;
	UPROPERTY()
	bool IsSliding;
	UPROPERTY()
	int PreviousAngle = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

public:
	UPROPERTY(EditDefaultsOnly, Category = Grapple)
		float DetectionRadius;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
		float GrappleThrowDistance;

private:
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

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
		UAnimMontage* GrappleAir;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
		UAnimMontage* GrappleGround;
};
