// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABPlayerController, Log, All)
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AABPlayerController();

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnScureChagnedCpp"))
	void K2_OnPlayerScoreChanged(int32 NewPlayerSocre);
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGmaeClearCpp"))
	void K2_OnGameClear();
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameOverCpp"))
	void K2_OnGameOver();
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameRetryCountCpp"))
	void K2_OnGameRetryCount(int32 NewGameRetryCount);

	void GameScoreChanged(int32 NewScore);
	void GameClear();
	void GameOver();

protected:
	virtual void BeginPlay() override;

	// HUD
protected:
	// 클래스 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD)
	TSubclassOf<class UABHUDWidget> ABHUDWidgetClass;

	// 생성항 오브젝트를 닮을 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD)
	TObjectPtr<class UABHUDWidget> ABHUDWidget;

	// Save Game Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Game)
	TObjectPtr<class UABSaveGame> SaveGameInstance;

};
