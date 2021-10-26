// Fill out your copyright notice in the Description page of Project Settings.


#include "AEGameMode.h"
#include "Blueprint/UserWidget.h"

void AAEGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidget);
	CurrentWidget->AddToViewport();
}
