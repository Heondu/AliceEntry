// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerController.h"
#include "AEPlayerCharacter.h"
#include "AEAliceCharacter.h"
#include "AEBunnyCharacter.h"

void AAEPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Tag", EInputEvent::IE_Pressed, this, &AAEPlayerController::ChangeCharacter);
}

void AAEPlayerController::ChangeCharacter()
{
	FVector Location = GetPawn()->GetActorLocation();
	FRotator Rotation = GetPawn()->GetActorRotation();

	AAEPlayerCharacter* NewCharacter;

	if (bFlag)
	{
		NewCharacter = GetWorld()->SpawnActor<AAEAliceCharacter>(AliceClass, Location, Rotation);
	}
	else
	{
		NewCharacter = GetWorld()->SpawnActor<AAEBunnyCharacter>(BunnyClass, Location, Rotation);
	}
	FRotator NewControlRotation = GetControlRotation();

	TArray<AActor*> Actors;
	GetPawn()->GetAttachedActors(Actors);
	
	for (AActor* Actor : Actors)
	{
		Actor->Destroy();
	}
	GetPawn()->Destroy();
	
	CHECK(nullptr != NewCharacter);
	
	Possess(NewCharacter);
	SetControlRotation(NewControlRotation);
	
	bFlag = !bFlag;
}
