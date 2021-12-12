// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AIController.h"
#include "AEAIController.generated.h"

/**
 * 
 */
UCLASS()
class ALICEENTRY_API AAEAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAEAIController();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName CanAttack;

	void RunAI();
	void StopAI();

	void SetTarget(UObject* Target);

private:
	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, Category = AI)
	class UBlackboardData* BBAsset;
};
