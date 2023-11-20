// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FIndPatrolPos.h"
#include "ABAI.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Interface/ABCharacterAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_FIndPatrolPos::UBTTask_FIndPatrolPos()
{

}

EBTNodeResult::Type UBTTask_FIndPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	IABCharacterAIInterface* AIPawn = Cast< IABCharacterAIInterface>(ControllingPawn);
	if (AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector Oregin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	float PatrollRadius = AIPawn->GetAIPatrollRadius();
	FNavLocation NextPatrolPos;

	if (NavSystem->GetRandomPointInNavigableRadius(Oregin, PatrollRadius, NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
