// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABStageGimmick.generated.h"

DECLARE_DELEGATE(FOnStageChangeDelegate);
USTRUCT(BlueprintType)
struct FStageChangeDelegateWrapper
{
	GENERATED_BODY()
	FStageChangeDelegateWrapper() { }
	FStageChangeDelegateWrapper(const FOnStageChangeDelegate& InDelegate) : StageDelegate(InDelegate) { }
	FOnStageChangeDelegate StageDelegate;
};

UENUM(BlueprintType)
enum class EStageState : uint8
{
	READY = 0,
	FIGHT,
	REWARD,
	NEXT
};


UCLASS()
class ARENABATTLE_API AABStageGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABStageGimmick();


public:
	FORCEINLINE int32 GetStageNum() const { return CurrentStateNum; }
	FORCEINLINE void SetStageNum(int32 NewStageNum) { CurrentStateNum = NewStageNum; }
protected:
	// 속성이 변경되면 호출됨 : 에디터에서 CurrentState 변경시 호출됨
	virtual void OnConstruction(const FTransform& Transform) override;

	// Stage Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Stage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> StageTrigger;

	UFUNCTION()
	void OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
		, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	// Gate Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<class UStaticMeshComponent>> Gates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UBoxComponent>> GateTriggers;

	UFUNCTION()
	void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
		, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OpenAllGates();
	void ClosedAllGates();
	// State Section
protected:
	UPROPERTY(EditAnywhere, Category = Stage, Meta = (AllowPrivateAccess = "true"))
	EStageState CurrentState;

	void SetState(EStageState InNewState);

	UPROPERTY()
	TMap< EStageState, FStageChangeDelegateWrapper> StateChangeActions;

	void SetReady();
	void SetFight();
	void SetchooseReward();
	void SetChooseNext();
	
	// Fight Section 
	UPROPERTY(EditAnywhere, Category = Fight, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABCharacterNonPlayer> OpponentClass; // TSubclassOf 상속받은 클래스 목록만 보임

	UPROPERTY(EditAnywhere, Category = Fight, Meta = (AllowPrivateAccess = "true"))
	float OpponentSpawnTime;

	UFUNCTION()
	void OnOpponentDestroyed(AActor* DestroyedActor);

	FTimerHandle OpponentTimerHandle;
	void OnOpponentSpawn();

	// Reward Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Reward, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABItemBox> RewardBoxClass;

	UPROPERTY(VisibleAnywhere, Category = Reward, Meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<class AABItemBox>> RewardBoxes; // TWeakObjectPtr 약참조 액터가 소멸될때 같이 소멸될때 사용

	TMap<FName, FVector> RewardBoxLocations;
	UFUNCTION()
	void OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
		, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SpawnRewardBoxes();

	// Stage Stat
protected:
	UPROPERTY(VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentStateNum;
};
