// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "AEAIController.h"
#include "AECharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAEBasicCharacter* AEBasicCharacter = Cast<AAEBasicCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AEBasicCharacter)
		return EBTNodeResult::Failed;

	AAEBasicCharacter* Target = Cast<AAEBasicCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAEAIController::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - AEBasicCharacter->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	AEBasicCharacter->SetActorRotation(FMath::RInterpTo(AEBasicCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
