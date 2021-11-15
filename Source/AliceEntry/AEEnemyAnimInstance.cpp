// Fill out your copyright notice in the Description page of Project Settings.


#include "AEEnemyAnimInstance.h"

void UAEEnemyAnimInstance::PlayAttackMontage()
{
	Super::PlayAttackMontage();

	Montage_Play(AttackMontages[FMath::RandRange(0, AttackMontages.Num() - 1)], 1.0f);
}