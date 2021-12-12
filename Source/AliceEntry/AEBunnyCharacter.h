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

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float AxisValue) override;
	virtual void MoveRight(float AxisValue) override;
	virtual void Jump() override;
	virtual void Attack() override;

private:
	void AttackMove();
	void Skill1();
	void Skill2();
	void Skill3();

	UPROPERTY(EditDefaultsOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackMoveForce;
};
