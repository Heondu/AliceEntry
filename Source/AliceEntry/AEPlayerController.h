// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "GameFramework/PlayerController.h"
#include "AEPlayerController.generated.h"

/**
 * 
 */

class AAEAliceCharacter;
class AAEBunnyCharacter;

UCLASS()
class ALICEENTRY_API AAEPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void ChangeCharacter();

	UPROPERTY()
	bool bFlag = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAEAliceCharacter> AliceClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAEBunnyCharacter> BunnyClass;

	UPROPERTY()
	float TagTime;
};
