// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	//
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayterControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayterControllerClassRef.Class)
	{
		PlayerControllerClass = PlayterControllerClassRef.Class;
	}

	ClearScore = 3;
	CurrentScore = 0;
	bIsCleard = false;
}

void AABGameMode::OnPlayerScoreChanged(int32 NewPlayerSocre)
{
	CurrentScore = NewPlayerSocre;

	AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ABPlayerController)
	{
		ABPlayerController->GameScoreChanged(CurrentScore);
	}

	if (CurrentScore > ClearScore)
	{
		bIsCleard = true;

		if (ABPlayerController)
		{
			ABPlayerController->GameClear();
		}
	}
}

void AABGameMode::OnPlayerDead()
{
	AABPlayerController* ABPlayerController = Cast<AABPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ABPlayerController)
	{
		ABPlayerController->GameOver();
	}
}

bool AABGameMode::IsGameCleared()
{
	return bIsCleard;
}
