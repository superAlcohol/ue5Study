// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"
#include "Character/ABCharacterNonPlayer.h"
#include "Item/ABItemBox.h"
#include "Interface/ABGameInterface.h"


// Sets default values
AABStageGimmick::AABStageGimmick()
{
	// Stage section
	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	// RootComponent 지정안하면 작동안함
	RootComponent = Stage;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if (StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	StageTrigger->SetCollisionProfileName(CPPROFILE_ABTRIGGER);
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// Gate section
	static FName GateSockets[] = { TEXT("+XGate"), TEXT("-XGate"), TEXT("+YGate"), TEXT("-YGate") };
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	for (FName GeteSocket : GateSockets)
	{
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GeteSocket);
		if (GateMeshRef.Object)
		{
			Gate->SetStaticMesh(GateMeshRef.Object);
			Gate->SetupAttachment(Stage, GeteSocket);
			Gate->SetRelativeLocation(FVector(0.0f, -85.0f, 0.0f));
			Gate->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
			Gates.Add(GeteSocket, Gate);
		}

		FName TriggerName = *GeteSocket.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));	
		GateTrigger->SetupAttachment(Stage, GeteSocket);
		GateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		GateTrigger->SetCollisionProfileName(CPPROFILE_ABTRIGGER);
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GeteSocket);

		GateTriggers.Add(GateTrigger);
	}

	// State Section
	CurrentState = EStageState::READY;
	StateChangeActions.Add(EStageState::READY, FStageChangeDelegateWrapper(FOnStageChangeDelegate::CreateUObject(this, &AABStageGimmick::SetReady)));
	StateChangeActions.Add(EStageState::FIGHT, FStageChangeDelegateWrapper(FOnStageChangeDelegate::CreateUObject(this, &AABStageGimmick::SetFight)));
	StateChangeActions.Add(EStageState::REWARD, FStageChangeDelegateWrapper(FOnStageChangeDelegate::CreateUObject(this, &AABStageGimmick::SetchooseReward)));
	StateChangeActions.Add(EStageState::NEXT, FStageChangeDelegateWrapper(FOnStageChangeDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext)));

	OpponentSpawnTime = 2.0f;
	OpponentClass = AABCharacterNonPlayer::StaticClass();

	// Reward section
	RewardBoxClass = AABItemBox::StaticClass();
	for (FName GateSocket : GateSockets)
	{
		FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}


	CurrentStateNum = 0;

}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetState(CurrentState);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetState(EStageState::FIGHT);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	check(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2)); // 왼쪽 2글자만 잘라냄
	check(Stage->DoesSocketExist(SocketName));

	FVector NewLocation = Stage->GetSocketLocation(SocketName);
	TArray<FOverlapResult> OverlapResults;
	// 소켓위치에 이미 무언가 설치되어있는지 체크
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(GateTrigger), false, this);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllStaticObjects,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

	// 아무것도 설치 되있지 않으면 기믹 설치
	if (false == bResult)
	{
		FTransform NewTransfrom(NewLocation);
		// 지연생성
		AABStageGimmick* NewGimmic = GetWorld()->SpawnActorDeferred<AABStageGimmick>(AABStageGimmick::StaticClass(), NewTransfrom);
		if (NewGimmic)
		{
			NewGimmic->SetStageNum(CurrentStateNum + 1);
			NewGimmic->FinishSpawning(NewTransfrom); // 지연생성 완료
		}
	}

}

void AABStageGimmick::OpenAllGates()
{
	for (auto Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AABStageGimmick::ClosedAllGates()
{
	for (auto Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator::ZeroRotator); // 기본값
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;

	if (StateChangeActions.Contains(InNewState))
	{
		StateChangeActions[InNewState].StageDelegate.ExecuteIfBound();
	}
}

void AABStageGimmick::SetReady()
{
	StageTrigger->SetCollisionProfileName(CPPROFILE_ABTRIGGER);
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	OpenAllGates();
}

void AABStageGimmick::SetFight()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	ClosedAllGates();
	// OpponentSpawnTime 시간 후 스폰함수 호출
	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);

}

void AABStageGimmick::SetchooseReward()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	ClosedAllGates();

	SpawnRewardBoxes();
}

void AABStageGimmick::SetChooseNext()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPPROFILE_ABTRIGGER);
	}

	OpenAllGates();
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	IABGameInterface* ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode());
	if (ABGameMode)
	{
		ABGameMode->OnPlayerScoreChanged(CurrentStateNum);
		if (ABGameMode->IsGameCleared())
		{
			return;
		}
	}

	SetState(EStageState::REWARD);
}

void AABStageGimmick::OnOpponentSpawn()
{
	// SpawnActor 생성되는 방식
	// const FVector SpawnLocation = GetActorLocation() + FVector::UpVector * 88.0f;
	// 이곳에서 이미 SpawnActor 호출되면서 BeginPlay() 함수가 호출되어 초기화 되서
	// SetLevel 세팅이 되기전에 값이 잘못 세팅됨
	// AActor* OpponentActor = GetWorld()->SpawnActor(OpponentClass, &SpawnLocation, &FRotator::ZeroRotator);
	// AABCharacterNonPlayer* ABOpponentCharacter = Cast<AABCharacterNonPlayer>(OpponentActor);

	// SpawnActorDeferred 지연생성 방식
	const FTransform SpawnTransform(GetActorLocation() + FVector::UpVector * 88.0f);
	AABCharacterNonPlayer* ABOpponentCharacter = GetWorld()->SpawnActorDeferred<AABCharacterNonPlayer>(OpponentClass, SpawnTransform);
	if (ABOpponentCharacter)
	{
		ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
		ABOpponentCharacter->SetLevel(CurrentStateNum); 
		// 해당 함수가 호출 된 후 캐릭터가 가지고있는것들의 BeginPlay() 함수 호출
		ABOpponentCharacter->FinishSpawning(SpawnTransform); 
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();
			if (OverlappedBox != ValidItemBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	SetState(EStageState::NEXT);
}

void AABStageGimmick::SpawnRewardBoxes()
{

	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		FTransform  SpawnTransfrom(GetActorLocation() + RewardBoxLocation.Value + FVector(0.0f, 0.0f, 30.0f));
		AABItemBox* RewardBoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, SpawnTransfrom);
		if (RewardBoxActor)
		{
			RewardBoxActor->Tags.Add(RewardBoxLocation.Key);
			RewardBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(RewardBoxActor);
		}
	}

	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			RewardBox.Get()->FinishSpawning(RewardBox.Get()->GetActorTransform());
		}
	}
}
