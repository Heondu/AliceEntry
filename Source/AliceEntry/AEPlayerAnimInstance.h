// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEAnimInstance.h"
#include "AEPlayerAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRollEndDelegate);

/**
 * 
 */
UCLASS()
class ALICEENTRY_API UAEPlayerAnimInstance : public UAEAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void PlayAttackMontage() override;
	void JumpToAttackMontageSection(int32 NewSection);
	void PlayRollAnim(bool bIsBack);

	FOnRollEndDelegate OnRollEnd;
private:
	FName GetAttackMontageSectionName(int32 Section);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, Meta = (AllowPrivateAccess = true))
	UAnimMontage* RollMontage;

	UFUNCTION()
	void AnimNotify_RollEnd() { OnRollEnd.Broadcast(); }
};
