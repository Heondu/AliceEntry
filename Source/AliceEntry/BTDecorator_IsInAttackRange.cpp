// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "AEAIController.h"
#include "AECharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return false;

	AAECharacter* Target = Cast<AAECharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAEAIController::TargetKey));
	if (nullptr == Target) return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.0f);
	return bResult;
}
