// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ABProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include <Player/ABPlayerController.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AABProjectileBase::AABProjectileBase()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
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

		Destroy();
	}
}

