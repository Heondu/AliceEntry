// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEBasicCharacter.h"
#include "AEGrapplePoint.h"
#include "GameFramework/Character.h"
#include "AEPlayerCharacter.generated.h"

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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void MoveForward(float AxisValue);
	virtual void MoveRight(float AxisValue);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Jump();
	void Grapple();

	float TurnRate;
	float LookRate;

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
