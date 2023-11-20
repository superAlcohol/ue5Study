// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTaskNode_TurnToTarget.h"
#include "ABAI.h"
#include "AIController.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTaskNode_TurnToTarget::UBTTaskNode_TurnToTarget()
{
	NodeName = TEXT("Turn");

}

EBTNodeResult::Type UBTTaskNode_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == TargetPawn)
	{
		return EBTNodeResult::Failed;
	}

	IABCharacterAIInterface* AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	float TurnSpeed = AIPawn->GetAITurnSpeed();
	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), TurnSpeed));

	return EBTNodeResult::Succeeded;
}
