// Fill out your copyright notice in the Description page of Project Settings.


#include "AEPlayerController.h"
#include "AEPlayerCharacter.h"
#include "AEAliceCharacter.h"
#include "AEBunnyCharacter.h"

void AAEPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TagTime = 1;
}

void AAEPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Tag", EInputEvent::IE_Pressed, this, &AAEPlayerController::ChangeCharacter);
}

void AAEPlayerController::ChangeCharacter()
{
	AAEPlayerCharacter* PlayerRef = Cast<AAEPlayerCharacter>(GetPawn());
	if (nullptr == PlayerRef) return;

	if (PlayerRef->EState != ECharacterState::Stopped &&
		PlayerRef->EState != ECharacterState::Walking &&
		PlayerRef->EState != ECharacterState::Running) return;

	float Health = PlayerRef->Health;

	FVector Location = PlayerRef->GetActorLocation();
	FRotator Rotation = PlayerRef->GetActorRotation();
	FRotator NewControlRotation = GetControlRotation();

	PlayerRef->Dissolve(TagTime);

	AAEPlayerCharacter* NewCharacter;
	
	if (bFlag)
	{
		NewCharacter = GetWorld()->SpawnActor<AAEAliceCharacter>(AliceClass, Location, Rotation);
	}
	else
	{
		NewCharacter = GetWorld()->SpawnActor<AAEBunnyCharacter>(BunnyClass, Location, Rotation);
	}
	
	CHECK(nullptr != NewCharacter);
	
	NewCharacter->Health = Health;
	Possess(NewCharacter);
	SetControlRotation(NewControlRotation);

	NewCharacter->Appearance(TagTime);
	
	bFlag = !bFlag;
}
