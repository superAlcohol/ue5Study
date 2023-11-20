// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/ABGameInterface.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase, public IABGameInterface
{
	GENERATED_BODY()

public:
	AABGameMode();

	virtual void OnPlayerScoreChanged(int32 NewPlayerSocre) override;
	virtual void OnPlayerDead() override;
	virtual bool IsGameCleared() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Game)
	int ClearScore;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly , Category = Game)
	int CurrentScore;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game)
	uint8 bIsCleard : 1;


};
