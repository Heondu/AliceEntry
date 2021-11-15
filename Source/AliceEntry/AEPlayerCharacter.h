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
	virtual void Attack();
	void Roll();
	
	void CamShake();

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ECharacterState> EState;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, Meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ShakeAttack;
};
