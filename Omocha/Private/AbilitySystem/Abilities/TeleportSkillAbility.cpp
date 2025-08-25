// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/TeleportSkillAbility.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "OmochaGameplayTags.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"

UTeleportSkillAbility::UTeleportSkillAbility()
{
	// 텔레포트 스킬용 설정
	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Normal_Skill_RMB);
	SetAssetTags(Tags);
}

void UTeleportSkillAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UTeleportSkillAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTeleportSkillAbility::ExecuteMovement(const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("Teleport ExecuteMovement called"));

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("Character is null in Teleport ExecuteMovement"));
		return;
	}

	FVector StartLocation = Character->GetActorLocation();

	// 위치 유효성 검증
	if (!IsLocationValid(TargetLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Teleport target location is invalid, cancelled"));
		return;
	}
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), StartLocation, TargetLocation, FColor::Cyan, false, 2.0f, 0, 5.0f);
	}

	if (UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent())
	{
		float CapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();

		if (DrawDebugType != EDrawDebugTrace::None)
		{
			DrawDebugCapsule(GetWorld(), TargetLocation, CapsuleHalfHeight, CapsuleRadius,
			                 FQuat::Identity, FColor::Cyan, false, 3.0f, 0, 3.0f);
		}
	}

	// 텔레포트 시작 이펙트 (현재 위치)
	FGameplayCueParameters StartCueParams;
	StartCueParams.Location = StartLocation;
	StartCueParams.SourceObject = Character;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
		FOmochaGameplayTags::Get().GameplayCue_Movement_Teleport,
		StartCueParams
	);

	// 딜레이 후 실제 텔레포트 실행
	if (TeleportDelay > 0.0f)
	{
		FTimerHandle TeleportTimer;
		GetWorld()->GetTimerManager().SetTimer(
			TeleportTimer,
			[this, TargetLocation]() { PerformTeleport(TargetLocation); },
			TeleportDelay,
			false
		);
	}
	else
	{
		PerformTeleport(TargetLocation);
	}
}

void UTeleportSkillAbility::PerformTeleport(const FVector& TargetLocation)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Performing teleport to: %s"), *TargetLocation.ToString());

	FGameplayCueParameters StartCueParams;
	StartCueParams.Location = Character->GetActorLocation();
	StartCueParams.SourceObject = Character;
	// 텔레포트 시작 이펙트
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
		FOmochaGameplayTags::Get().GameplayCue_Movement_Teleport,
		StartCueParams
	);

	// 실제 텔레포트 (나중에 Movement Component로 변경)
	Character->SetActorLocation(TargetLocation);

	// 텔레포트 도착 이펙트
	FGameplayCueParameters EndCueParams;
	EndCueParams.Location = TargetLocation;
	EndCueParams.SourceObject = Character;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
		FOmochaGameplayTags::Get().GameplayCue_Movement_Teleport,
		EndCueParams
	);
}
