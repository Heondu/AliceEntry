// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEAnimInstance.h"
#include "AEPlayerCharacter.h"
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
	virtual void NativeBeginPlay() override;
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

	UPROPERTY()
	class AAEPlayerCharacter* PlayerRef;
	
	UFUNCTION()
	void AnimNotify_RollEnd() { OnRollEnd.Broadcast(); }

	UFUNCTION()
	void AnimNotify_StartMovement() { PlayerRef->StartGrapplingMovement(); }

	UFUNCTION()
	void AnimNotify_RopeAppear() { PlayerRef->RopeVisibility(true); }

	UFUNCTION()
	void AnimNotify_RopeDisappear() { PlayerRef->RopeVisibility(false); }

	UFUNCTION()
	void AnimNotify_AnimEnd() { PlayerRef->ResetMovement(); }
};
