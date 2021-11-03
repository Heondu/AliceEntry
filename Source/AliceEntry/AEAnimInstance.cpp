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
	PlayerRef = Cast<AAEPlayerCharacter>(TryGetPawnOwner());
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
	Montage_Play(AttackMontage, 1.0f);
}

void UAEAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	CHECK(!IsDead);
	CHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UAEAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UAEAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UAEAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UAEAnimInstance::AnimNotify_StartMovement()
{
	PlayerRef->StartGrapplingMovement();
}

void UAEAnimInstance::AnimNotify_RopeAppear()
{
	PlayerRef->RopeVisibility(true);
}

void UAEAnimInstance::AnimNotify_RopeDisappear()
{
	PlayerRef->RopeVisibility(false);
}

void UAEAnimInstance::AnimNotify_AnimEnd()
{
	PlayerRef->ResetMovement();
}
