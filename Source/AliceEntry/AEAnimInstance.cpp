// Fill out your copyright notice in the Description page of Project Settings.


#include "AEAnimInstance.h"
#include "AEPlayerCharacter.h"

UAEAnimInstance::UAEAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
}

void UAEAnimInstance::NativeBeginPlay()
{
}

void UAEAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		ACharacter* Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UAEAnimInstance::PlayAttackMontage()
{
	CHECK(!IsDead);
}

void UAEAnimInstance::PlayHitAnim()
{
	CHECK(!IsDead);
	Montage_Play(HitMontage, 1.0f);
}

void UAEAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UAEAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UAEAnimInstance::AnimNotify_AttackEnd()
{
	OnAttackEnd.Broadcast();
}

void UAEAnimInstance::AnimNotify_HitEnd()
{
	OnHitEnd.Broadcast();
}

void UAEAnimInstance::AnimNotify_AttackMove()
{
	OnAttackMove.Broadcast();
}