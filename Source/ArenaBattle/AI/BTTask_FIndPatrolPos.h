// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FIndPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTask_FIndPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FIndPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
