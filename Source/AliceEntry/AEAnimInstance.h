// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "Animation/AnimInstance.h"
#include "AEAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHitAnimEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

/**
 * 
 */
UCLASS()
class ALICEENTRY_API UAEAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UAEAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);

	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnHitAnimEndDelegate OnHitAnimEnd;
	FOnAttackEndDelegate OnAttackEnd;
	void SetDeadAnim() { IsDead = true; }
	void PlayHitAnim();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Anim, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Anim, Meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim, Meta = (AllowPrivateAccess = true))
	UAnimMontage* HitMontage;

	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;

	UPROPERTY()
	class AAEPlayerCharacter* PlayerRef;

	UFUNCTION()
	void AnimNotify_StartMovement();

	UFUNCTION()
	void AnimNotify_RopeAppear();

	UFUNCTION()
	void AnimNotify_RopeDisappear();

	UFUNCTION()
	void AnimNotify_AnimEnd();

	UFUNCTION()
	void AnimNotify_HitAnimEnd();

	UFUNCTION()
	void AnimNotify_AttackEnd();
};
