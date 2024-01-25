// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ARENABATTLE_API AABProjectileBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
public:	
	// Sets default values for this actor's properties
	AABProjectileBase();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(VisibleAnywhere, Category = Damage)
	float AttackDamage;

	//protected:
 	// niaagra C++ Call Test
	UPROPERTY(EditAnywhere, Category = "Bullet")
	TObjectPtr<class UNiagaraSystem> ImpectEffect;

	UPROPERTY(EditAnywhere, Category = "Fire Sound")
	TObjectPtr<USoundBase> HitSoundBase;
};
