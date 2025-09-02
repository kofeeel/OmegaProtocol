// Copyright It's My Turn


#include "AbilitySystem/Abilities/RustyBasicAttack.h"

#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "GameFramework/Character.h"

URustyBasicAttack::URustyBasicAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

void URustyBasicAttack::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bHasProcessedTargetData = true;

	FVector TargetLocation = FVector::ZeroVector;
	if (TargetDataHandle.Num() > 0)
	{
		const FHitResult* HitResult = TargetDataHandle.Get(0)->GetHitResult();
		if (HitResult && HitResult->bBlockingHit)
		{
			TargetLocation = HitResult->ImpactPoint;
		}
		else if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			TargetLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 1000.0f;
		}
	}

	if (MouseClickTask && IsValid(MouseClickTask))
	{
		MouseClickTask->ValidData.RemoveDynamic(this, &URustyBasicAttack::OnTargetDataReady);
		MouseClickTask->EndTask();
		MouseClickTask = nullptr;
	}

	StartAttackSequence(TargetLocation);
    bHasProcessedTargetData = false;
}
