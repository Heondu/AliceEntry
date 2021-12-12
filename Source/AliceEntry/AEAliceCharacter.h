// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEPlayerCharacter.h"
#include "AEAliceCharacter.generated.h"

/**
 * 
 */

class AAEGun;

UCLASS()
class ALICEENTRY_API AAEAliceCharacter : public AAEPlayerCharacter
{
	GENERATED_BODY()

public:
	AAEAliceCharacter();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void Attack() override;

private:
	void Shoot();
	void Skill1();
	void Skill2();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAEGun> GunClass;

	UPROPERTY()
	AAEGun* GunL;

	UPROPERTY()
	AAEGun* GunR;

	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;
};