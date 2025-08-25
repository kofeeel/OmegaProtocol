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

void URustyBasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	bHasProcessedTargetData = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	RequestTargetData();
}

void URustyBasicAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                   bool bWasCancelled)
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask))
	{
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URustyBasicAttack::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive())
	{
		bShouldEndAttack = true;
		OnInputReleased();
	}

	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void URustyBasicAttack::RequestTargetData()
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask))
	{
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &URustyBasicAttack::OnTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	CurrentTargetDataTask = UOmochaMouseHitTask::CreateTargetDataUnderMouse(this);
	if (CurrentTargetDataTask)
	{
		CurrentTargetDataTask->ValidData.AddDynamic(this, &URustyBasicAttack::OnTargetDataReady);
		CurrentTargetDataTask->ReadyForActivation();
	}
}

void URustyBasicAttack::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (bHasProcessedTargetData)
	{
		return;
	}

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

	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask))
	{
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &URustyBasicAttack::OnTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	StartAttackSequence(TargetLocation);
}
