// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		// MeshComp가 IABAnimationAttackInterface의 AttackHitCheck를 구현하지않으면 AttackPawn가 없음
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}

}
