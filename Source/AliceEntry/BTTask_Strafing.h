// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Strafing.generated.h"

/**
 * 
 */
UCLASS()
class ALICEENTRY_API UBTTask_Strafing : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Strafing();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
