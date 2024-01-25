// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ABProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include <Player/ABPlayerController.h>
#include <Kismet/GameplayStatics.h>
#include "../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

// Sets default values
AABProjectileBase::AABProjectileBase()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// set up a notification for when this component hits something blocking
	CollisionComp->OnComponentHit.AddDynamic(this, &AABProjectileBase::OnHit);		

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component 
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	AttackDamage = 30;

	// 나이아가라 C++ 호출 테스트
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticleSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/ArenaBattle/Effect/NiagaraEffect/NS_ImpectEffect.NS_ImpectEffect'"));
	if (ParticleSystem.Succeeded()) 
	{
		ImpectEffect = ParticleSystem.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> HitSoundAsset(TEXT("/Script/MetasoundEngine.MetaSoundSource'/Game/ArenaBattle/Sounds/MS_ImpectEffect.MS_ImpectEffect'"));
	if (HitSoundAsset.Succeeded())
		HitSoundBase = HitSoundAsset.Object;

	// HitSoundBase = LoadObject<USoundWave>(nullptr, TEXT("/Game/Sounds/MS_ImpectEffect.MS_ImpectEffect"));
}

void AABProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) /*&& OtherComp->IsSimulatingPhysics()*/)
	{
		// IsSimulatingPhysics() 활성화되어있는 것들적용
		// OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		 
		GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Orange, FString::Printf(TEXT("OnHit AttackDamage : %f"), AttackDamage) );

		AController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0); // GetController();
		if (nullptr != controller)
		{
			UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, controller, nullptr, NULL);
		}


		// 나이아가라 C++ 호출 테스트
		if (ImpectEffect)
		{
			FVector_NetQuantize ImpectPoint = Hit.ImpactPoint; // 충돌지점
			//FVector_NetQuantize Location = Hit.Location;
			// This spawns the chosen effect on the owning WeaponMuzzle SceneComponent
			UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ImpectEffect, OtherComp, NAME_None, ImpectPoint, FRotator(0), EAttachLocation::Type::KeepWorldPosition, true, true, ENCPoolMethod::None, true);
		}

		// UGameplayStatics::PlaySound2D(OtherActor, HitSoundBase);
		UGameplayStatics::PlaySoundAtLocation(OtherActor, HitSoundBase, OtherActor->GetActorLocation());
		Destroy();
	}  
}

