// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ABCharacterBase.h" // Corresponding header file alawy first

#include "ABCharacterControlData.h"
#include "ABComboActionData.h"
#include "Animation/AnimMontage.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/ABItems.h"
#include "UI/ABHpBarWidget.h"
#include "Physics/ABCollision.h"
#include "UI/ABWidgetComponent.h"
#include <Misc/OutputDeviceNull.h>
#include "Projectile/ABProjectileBase.h"
//#include "../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
//#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
//#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

DEFINE_LOG_CATEGORY(LogABCharacter);


// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPPROFILE_ABCAPSULE);

	// movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;


	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// 클래스정보를 가저오려면 레퍼런스 값뒤에 _C 
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// 캐릭터 애니메이션 map 컨테이너 추가
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuaterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> FpsDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Fps.ABC_Fps'"));
	if (FpsDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Fps, FpsDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
	if (ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> RifleShootMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_RifleShoot.AM_RifleShoot'"));
	if (RifleShootMontageRef.Object)
	{
		RifleShootMontage = RifleShootMontageRef.Object;
	}

	// 나이아가라 C++ 호출 테스트
	//static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticleSystem(TEXT("/Game/ArenaBattle/Effect/NiagaraEffect/NS_bulletEffect.NS_bulletEffect"));
	//if (ParticleSystem.Succeeded()) {
	//	BulletEffect = ParticleSystem.Object;
	//}

	// Stat Component
	CharcterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));
	// Widget Component
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Item Actions
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipRifle)));
	

	// Weaopon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
	
	MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);
	// Blueprint - BP_Bullet 
	// static ConstructorHelpers::FObjectFinder<UBlueprint> blueprint_finder(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/Bullet/BP_Bullet.BP_Bullet'"));
	// C++ - AABProjectileBase
	static ConstructorHelpers::FObjectFinder<UBlueprint> blueprint_finder(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/Bullet/BP_BulletCpp.BP_BulletCpp'"));
   	if (blueprint_finder.Object)
	{   
		BulletBP = blueprint_finder.Object->GeneratedClass;
	}

	//static ConstructorHelpers::FObjectFinder<UBlueprint> blueprint_projectile(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/Bullet/BP_BulletCpp.BP_BulletCpp'"));
	//if (blueprint_projectile.Object)
	//{
	//	ProjectileClass = blueprint_projectile.Object->GeneratedClass;
	//}

}

void AABCharacterBase::PostInitializeComponents()
{
	// begin play 이전에 발동
	Super::PostInitializeComponents();

	// 죽는 모션 델리게이트 등록
	CharcterStat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	CharcterStat->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);

}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	// Pawn 
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// Character movement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
	  
}

void AABCharacterBase::ProcessComboCommand()
{
	if (true == GetCharacterMovement()->IsFalling())
	{
		UE_LOG(LogABCharacter, Log, TEXT("bPressedJump is %d"), bPressedJump);
		return;
	}

	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}
	// 타이머가 비활성화 , 콤보가 끝났거나 타이밍을 놓쳐서 콤보가 끊긴경우
	if (false == ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	CurrentCombo = 1;

	// 이동기능 제거 후 콤보 구현
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// Animatiuon Setting
	const float AttackSpeedRate = CharcterStat->GetTotalStat().AttackSpeed;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool isProperlyEnded)
{
	ensure(CurrentCombo != 0);

	CurrentCombo = 0;
	// 이동 활성화
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	NotifyComboActionEnd();
}

void AABCharacterBase::NotifyComboActionEnd()
{
}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ConboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ConboIndex));

	const float AttackSpeedRate = CharcterStat->GetTotalStat().AttackSpeed;
	float ComboEffeciveTime = (ComboActionData->EffectiveFrameCount[ConboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffeciveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffeciveTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	// ComboTimerHandle 비활성화
	ComboTimerHandle.Invalidate();
	
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	// SCENE_QUERY_STAT(Attack) Attack 테그, false 복잡한 충돌처리할지, 충돌 제외할오브젝트 this(나자신)
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = CharcterStat->GetTotalStat().AttackRange;
	const float AttackRadius = CharcterStat->GetAttackRadius();
	const float AttackDamage = CharcterStat->GetTotalStat().Attack;
	FVector Start;
	FVector End;
	bool HitDetected = false;

	if (EquipItemType == EITemType::Weapon)
	{
		// 액션과 동시에 캡슐 충돌처리  
		Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
		End = Start + GetActorForwardVector() * AttackRange;
		HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);


		if (HitDetected)
		{
			FDamageEvent DamageEvent;
			OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}

		UE_LOG(LogABCharacter, Log, TEXT("HitDetected is %d"), HitDetected);


#if ENABLE_DRAW_DEBUG
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius,
			FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
#endif
	}
	else
	{
		Fire();
		// 나이아가라 C++ 호출 테스트
		//if (BulletEffect)
		//{
		//	// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
		//	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BulletEffect, Weapon, NAME_None, Start, FRotator(0), EAttachLocation::Type::KeepRelativeOffset, true, true, ENCPoolMethod::None, true);
		//	// Parameters can be set like this (see documentation for further info) - the names and type must match the user exposed parameter in the Niagara System
		//	NiagaraComponent->SetNiagaraVariableVec3(FString("BulletEnd"), End);
		//}
	}
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CharcterStat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); // 움직임 중지
	PlayDeadAnimation();			//  죽는 애니메이션 추가
	SetActorEnableCollision(false); // 충돌기능 제거
	HpBar->SetHiddenInGame(true);	//  UI 숨김
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if(HpBarWidget)
	{
		HpBarWidget->UpdateStat(CharcterStat->GetBaseStat(), CharcterStat->GetModifierStat());
		HpBarWidget->UpdateHpBar(CharcterStat->GetCurrentHp());
		CharcterStat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
		CharcterStat->OnStatChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateStat);
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	UABPotionItemData* PotionItemnData = Cast<UABPotionItemData>(InItemData);
	if (PotionItemnData)
	{
		CharcterStat->HealHep(PotionItemnData->HealAmount);
	}
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	UABWeaponItemData* WepoonItemData = Cast< UABWeaponItemData>(InItemData);
	if (WepoonItemData)
	{
		// ItemType change
		EquipItemType = InItemData->Type;
		// ABP_ABCharacter AnimGraph Event call ChangeWeapon
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		FOutputDeviceNull pAR;
		AnimInstance->CallFunctionByNameWithArguments(TEXT("ChangeWeapon"), pAR, nullptr, true);

		// 로딩되어있지않으면 
		if (WepoonItemData->WeaponMesh.IsPending())
		{
			WepoonItemData->WeaponMesh.LoadSynchronous(); // 로딩 시도 
		}

		Weapon->SetSkeletalMesh(WepoonItemData->WeaponMesh.Get());
		CharcterStat->SetModifierStat(WepoonItemData->ModifierStat);
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	UABScrollItemData* ScrollItemnData = Cast<UABScrollItemData>(InItemData);
	if (ScrollItemnData)
	{
		CharcterStat->AddBaseStat(ScrollItemnData->BaseStat);
	}
}

void AABCharacterBase::EquipRifle(UABItemData* InItemData)
{
	UABRifleItemData* RifleItemData = Cast< UABRifleItemData>(InItemData);
	if (RifleItemData)
	{
		// ItemType change
		EquipItemType = InItemData->Type;
		// ABP_ABCharacter AnimGraph Event call ChangeWeapon
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		FOutputDeviceNull pAR;
		AnimInstance->CallFunctionByNameWithArguments(TEXT("ChangeWeapon"), pAR, nullptr, true);

		// 로딩되어있지않으면 
		if (RifleItemData->WeaponMesh.IsPending())
		{
			RifleItemData->WeaponMesh.LoadSynchronous(); // 로딩 시도 
		}

		Weapon->SetSkeletalMesh(RifleItemData->WeaponMesh.Get());
		CharcterStat->SetModifierStat(RifleItemData->ModifierStat);
	}
}

int32 AABCharacterBase::GetLevel()
{
	return CharcterStat->GetCurrentLevel();
}

void AABCharacterBase::SetLevel(int32 InNewLevel)
{
	CharcterStat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BastStat, const FABCharacterStat& ModifierStat)
{
	float MovementSpeed = (BastStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

bool AABCharacterBase::isHitActor(bool InIsHit)
{
	return InIsHit;
}

void AABCharacterBase::Fire()
{
	if (GetController() == nullptr)
	{
		return;
	}

	isFireReady = true;

	// Try and fire a projectile
	UWorld* const World = GetWorld();
	if (BulletBP != nullptr && World != nullptr)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		FVector Start; // Weapon.Get()->GetForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius()
		Start = Weapon.Get()->GetSocketLocation("FireSocket");
		// GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius()
	
		const FVector SpawnLocation = Start + SpawnRotation.RotateVector(MuzzleOffset);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// Spawn the projectile at the muzzle
		World->SpawnActor<AActor>(BulletBP, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}
}

void AABCharacterBase::FireAction()
{
	if (isFireReady == false)
	{
		return;
	}

	isFireReady = false;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (nullptr != AnimInstance)
	{
		FName Section = TEXT("RifleFire");
		if (GetCharacterMovement()->IsWalking())
		{
			Section = TEXT("RifleFireRun");
			AnimInstance->Montage_JumpToSection(Section, RifleShootMontage);
		}
		else
		{
			Section = TEXT("RifleFire");
			AnimInstance->Montage_JumpToSection(Section, RifleShootMontage);
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		}

		UE_LOG(LogABCharacter, Log, TEXT("Fire Bullet : %s"), Section);

		const float AttackSpeedRate = CharcterStat->GetTotalStat().AttackSpeed;
		AnimInstance->Montage_Play(RifleShootMontage, AttackSpeedRate);
	}
}
