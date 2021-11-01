// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEPlayerCharacter.h"
#include "AEBunnyCharacter.generated.h"

/**
 * 
 */

UCLASS()
class ALICEENTRY_API AAEBunnyCharacter : public AAEPlayerCharacter
{
	GENERATED_BODY()

public:
	AAEBunnyCharacter();

	virtual void PostInitializeComponents() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float AxisValue) override;
	virtual void MoveRight(float AxisValue) override;
};
