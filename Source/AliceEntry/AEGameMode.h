// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Image.h"
#include "AEGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ALICEENTRY_API AAEGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void ShowGameOver();
	UImage* GetCrosshairImage();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ShowOptions();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	UUserWidget* HUDWidgetRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	UUserWidget* OptionsWidgetRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> OptionsWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> GameOverWidget;

	UPROPERTY()
	APlayerController* PlayerRef;

	UPROPERTY()
	UImage* Crosshair;
};
