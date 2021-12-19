// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "AEGrapplePoint.generated.h"

UCLASS()
class ALICEENTRY_API AAEGrapplePoint : public AActor
{
	GENERATED_BODY()
	
public:
	AAEGrapplePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Activate(class AAEPlayerCharacter* Player, bool bCanGrapple);
	void Deactivate();
	void CheckDistanceFromPlayer();
	void Use();
	void Reactive();

	UPROPERTY()
	bool bCanGrapple;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Grapple)
	UUserWidget* WidgetRef;

	UPROPERTY()
	class AAEPlayerCharacter* PlayerRef;

	UPROPERTY()
	bool bActive;

	UPROPERTY()
	bool bUsed;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	float MaxImageSize;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
	float MinImageSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Grapple)
	USphereComponent* DetectionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Grapple)
	UBoxComponent* DeactivateZoneComp;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
