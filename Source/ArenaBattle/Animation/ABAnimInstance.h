// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABAnimInstance();

protected:
	// 처음 한번 초기화때 호출
	virtual void NativeInitializeAnimation() override;
	// 시간마다 업데이트 호출
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshold;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshold;

};
