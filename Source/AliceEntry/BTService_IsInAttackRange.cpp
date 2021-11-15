// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsInAttackRange.h"
#include "AEAIController.h"
#include "AEPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
	Interval = 1.0f;
}

void UBTService_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	AAEPlayerCharacter* Target = Cast<AAEPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAEAIController::TargetKey));
	if (nullptr == Target) return;

	bool bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.0f);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AAEAIController::CanAttack, bResult);

}