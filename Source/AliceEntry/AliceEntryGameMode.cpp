// Copyright Epic Games, Inc. All Rights Reserved.

#include "AliceEntryGameMode.h"
#include "AliceEntryCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAliceEntryGameMode::AAliceEntryGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
