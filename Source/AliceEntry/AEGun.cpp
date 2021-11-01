// Fill out your copyright notice in the Description page of Project Settings.


#include "AEGun.h"
#include "DrawDebugHelpers.h"
#include "AEAliceCharacter.h"

// Sets default values
AAEGun::AAEGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AAEGun::PullTrigger()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlash"));
}

// Called when the game starts or when spawned
void AAEGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAEGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

