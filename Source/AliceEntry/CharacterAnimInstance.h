// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ALICEENTRY_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void PlayAttackAnim();

private:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackMontage;
};
