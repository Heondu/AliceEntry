// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AEAnimInstance.h"
#include "AEEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ALICEENTRY_API UAEEnemyAnimInstance : public UAEAnimInstance
{
	GENERATED_BODY()

public:
	virtual void PlayAttackMontage() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, Meta = (AllowPrivateAccess = true))
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, Meta = (AllowPrivateAccess = true))
	UAnimMontage* JumpAttackMontage;
};
