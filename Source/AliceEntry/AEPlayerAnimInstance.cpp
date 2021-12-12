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

void UAEPlayerAnimInstance::PlayRollAnim(FName Name)
{
	CHECK(!IsDead);
	if (Montage_IsPlaying(RollMontage)) return;

	Montage_Play(RollMontage, 1.0f);
	Montage_JumpToSection(Name, RollMontage);
}

void UAEPlayerAnimInstance::PlaySlideAnim()
{
	CHECK(!IsDead);
	
	Montage_Play(SlideMontage, 1.0f);
}

void UAEPlayerAnimInstance::PlayGrappleAnim(bool bIsAir)
{
	CHECK(!IsDead);

	if (bIsAir)
	{
		Montage_Play(GrappleAir);
	}
	else
	{
		Montage_Play(GrappleGround);
	}

}

void UAEPlayerAnimInstance::PlaySkillAnim(FName Name)
{
	CHECK(!IsDead);
	if (Montage_IsPlaying(SkillMontage)) return;

	Montage_Play(SkillMontage, 1.0f);
	Montage_JumpToSection(Name, SkillMontage);
}
