// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABRifleItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABRifleItemData : public UABItemData
{
	GENERATED_BODY()
	
public:
	UABRifleItemData();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;	// soft loading

	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;

};