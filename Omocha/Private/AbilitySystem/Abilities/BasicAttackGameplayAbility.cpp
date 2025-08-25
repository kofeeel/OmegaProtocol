// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/BasicAttackGameplayAbility.h"

#include "OmochaGameplayTags.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "Actor/OmochaProjectile.h"
#include "GameFramework/Character.h"

UBasicAttackGameplayAbility::UBasicAttackGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Ability_LMB;
	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Normal_Attack);
	SetAssetTags(Tags);
	DamageType = FOmochaGameplayTags::Get().Abilities_Type_None;

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

void UBasicAttackGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	bHasProcessedTargetData = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//RequestTargetData();
}

void UBasicAttackGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             bool bReplicateEndAbility, bool bWasCancelled)
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBasicAttackGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive()) {
		bShouldEndAttack = true;
		OnInputReleased();
	}

	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UBasicAttackGameplayAbility::RequestTargetData()
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &UBasicAttackGameplayAbility::OnTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	CurrentTargetDataTask = UOmochaMouseHitTask::CreateTargetDataUnderMouse(this);
	if (CurrentTargetDataTask) {
		CurrentTargetDataTask->ValidData.AddDynamic(this, &UBasicAttackGameplayAbility::OnTargetDataReady);
		CurrentTargetDataTask->ReadyForActivation();
	}
}

void UBasicAttackGameplayAbility::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// TODO: Refine aiming logic with a more accurate Line-Plane Intersection method in the future.

	if (bHasProcessedTargetData) {
		return;
	}
	bHasProcessedTargetData = true;

	FVector FloorHitLocation = FVector::ZeroVector;
	if (TargetDataHandle.Num() > 0) {
		const FHitResult* HitResult = TargetDataHandle.Get(0)->GetHitResult();
		if (HitResult && HitResult->bBlockingHit) {
			FloorHitLocation = HitResult->ImpactPoint;
		}
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	APlayerController* PC = GetCurrentActorInfo()->PlayerController.Get();

	FVector CorrectedTargetLocation = FloorHitLocation;

	if (Character && PC && PC->PlayerCameraManager) {
		const FVector MuzzleLocation = GetLaunchLocation(Character);
		const float SocketHeight = MuzzleLocation.Z;

		const FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();

		FVector CameraToFloorDirection = CameraLocation - FloorHitLocation;
		CameraToFloorDirection.Normalize();

		const FVector Offset = CameraToFloorDirection * SocketHeight;
		CorrectedTargetLocation = FloorHitLocation + Offset;
	}

	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &UBasicAttackGameplayAbility::OnTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	SettingMaxRange();
	StartAttackSequence(CorrectedTargetLocation);
}
