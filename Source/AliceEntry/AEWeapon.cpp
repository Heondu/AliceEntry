// Fill out your copyright notice in the Description page of Project Settings.


#include "AEWeapon.h"

// Sets default values
AAEWeapon::AAEWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	RootComponent = Weapon;

	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

