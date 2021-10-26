// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "GameFramework/Actor.h"
#include "AEWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class ALICEENTRY_API AAEWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAEWeapon();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
};
