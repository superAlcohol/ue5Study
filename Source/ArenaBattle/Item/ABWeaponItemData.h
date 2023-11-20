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
	// TObjectPtr<USkeletalMesh> WeaponMesh;	// �ϵ� ���۷��� �ε� �۽��۽� �ε� ��������ʾƵ� �޸𸮿� �ִ�.	
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;	// ����Ʈ ���۷��� �ε� �ʿ��� ������ �ε�
	
	UPROPERTY(EditAnywhere, Category = Stat)
	FABCharacterStat ModifierStat;

};
