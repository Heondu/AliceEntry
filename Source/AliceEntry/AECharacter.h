// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEBasicCharacter.h"
#include "Engine/DataTable.h"
#include "AECharacter.generated.h"

/**
 * 
 */

UCLASS()
class ALICEENTRY_API AAECharacter : public AAEBasicCharacter
{
	GENERATED_BODY()

public:
	AAECharacter();

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	class USpringArmComponent* BoomArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	class UCameraComponent* Camera;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	float TurnRate;
	float LookRate;
};
