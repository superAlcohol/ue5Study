// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbCharacterStat.h"
#include "ABGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UABGameSingleton();

	static UABGameSingleton& Get(); // Getinstance();

public:
	// index 값이라 -1 처리
	FORCEINLINE FABCharacterStat GetCharacterStat(int32 InLevel) const {
		return CharacterStatTable.IsValidIndex(InLevel -1) ? CharacterStatTable[InLevel-1] : FABCharacterStat();
	}

	UPROPERTY()
	int32 CharacterMaxLevel; // 총몇개의 레벨이 있는지
private:
	TArray<FABCharacterStat> CharacterStatTable;
};
