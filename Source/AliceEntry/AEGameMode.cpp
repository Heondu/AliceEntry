// Fill out your copyright notice in the Description page of Project Settings.


#include "AEGameMode.h"
#include "Blueprint/UserWidget.h"

void AAEGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidget);
	CurrentWidget->AddToViewport();

	OptionsWidgetRef = CreateWidget<UUserWidget>(GetWorld(), OptionsWidget);
	OptionsWidgetRef->AddToViewport();
	OptionsWidgetRef->SetVisibility(ESlateVisibility::Hidden);

	PlayerRef = GetWorld()->GetFirstPlayerController();
	PlayerRef->InputComponent->BindAction("Options", IE_Pressed, this, &AAEGameMode::ShowOptions);
}

void AAEGameMode::ShowOptions()
{
	if (OptionsWidgetRef->Visibility == ESlateVisibility::Visible)
	{
		OptionsWidgetRef->SetVisibility(ESlateVisibility::Hidden);
		PlayerRef->SetShowMouseCursor(false);
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		return;
	}
	OptionsWidgetRef->SetVisibility(ESlateVisibility::Visible);
	PlayerRef->SetShowMouseCursor(true);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AAEGameMode::ShowGameOver()
{
	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidget);
	CurrentWidget->AddToViewport();
	PlayerRef->SetShowMouseCursor(true);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}
