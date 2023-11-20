// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABItemData.h"
#include "GameData/ABCharacterStat.h"
#include "ABWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABWeaponItemData : public UABItemData
{
	GENERATED_BODY()

public:
	UABWeaponItemData();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Weapon)
	// TObjectPtr<USkeletalMesh> WeaponMesh;	// 하드 레퍼런스 로딩 앱시작시 로딩 사용하지않아도 메모리에 있다.	
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;	// 소프트 레퍼런스 로딩 필요한 순간에 로딩
	
	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;

};
