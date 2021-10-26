// Fill out your copyright notice in the Description page of Project Settings.


#include "AEAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AAEAIController::HomePosKey(TEXT("HomePos"));
const FName AAEAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AAEAIController::TargetKey(TEXT("Target"));

AAEAIController::AAEAIController()
{
}

void AAEAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
}

void AAEAIController::RunAI()
{
	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			LOG(Error, TEXT("AIController couldn't run behavior tree!"));
		}
	}
}

void AAEAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr == BehaviorTreeComponent) return;
	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}
