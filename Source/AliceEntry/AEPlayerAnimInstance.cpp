// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerAnimInstance.h"

void UAEPlayerAnimInstance::NativeBeginPlay()
{
	PlayerRef = Cast<AAEPlayerCharacter>(TryGetPawnOwner());
}

void UAEPlayerAnimInstance::PlayAttackMontage()
{
	Super::PlayAttackMontage();

	Montage_Play(AttackMontage, 1.0f);
}

void UAEPlayerAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	CHECK(!IsDead);
	CHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

FName UAEPlayerAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UAEPlayerAnimInstance::PlayRollAnim(bool bIsBack)
{
	CHECK(!IsDead);
	if (Montage_IsPlaying(RollMontage)) return;

	Montage_Play(RollMontage, 1.0f);
	if (bIsBack)
	{
		Montage_JumpToSection(FName("Back"), RollMontage);
	}
}
