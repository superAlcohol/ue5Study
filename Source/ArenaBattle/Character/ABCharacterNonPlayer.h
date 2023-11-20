// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Character/ABCharacterBase.h"
#include "Interface/ABCharacterAIInterface.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
 // config ArenaBattle사용 지정하면 config 폴더에 DefaultArenaBattle.ini 사용
UCLASS(config=ArenaBattle)
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase, public IABCharacterAIInterface
{
	GENERATED_BODY()

public:
	AABCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;

protected:
	void SetDead() override;
	
	void NPCMeshLoadCompleted();

	// config 사용하겠다 선언
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	TSharedPtr<FStreamableHandle> NPCMeshHandle;

	// AI
protected:
	virtual float GetAIPatrollRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	virtual void NotifyComboActionEnd() override;
};
