// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/DashSkillAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "OmochaGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"


UDashSkillAbility::UDashSkillAbility()
{
	// 대쉬 스킬용 설정
	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Normal_Attack); // Q키로 설정
	SetAssetTags(Tags);

	// 기본값 설정
	DashDuration = 0.3f;
	DashCurve = nullptr;
}

void UDashSkillAbility::ExecuteMovement(const FVector& TargetLocation)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("Character is null in Dash ExecuteMovement"));
		return;
	}

	FVector StartLocation = Character->GetActorLocation();
	float Distance = CalculateActualDistance(TargetLocation);

	// 위치 유효성 검증
	if (!IsLocationValid(TargetLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Dash target location is invalid, cancelled"));
		return;
	}

	if (DrawDebugType != EDrawDebugTrace::None)
	{
		DrawDebugLine(GetWorld(), StartLocation, TargetLocation, FColor::Orange, false, DashDuration + 1.0f, 0, 5.0f);
	}
	if (UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent())
	{
		float CapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();

		if (DrawDebugType != EDrawDebugTrace::None)
		{
			// 대쉬 목표 위치 표시 (주황색)
			DrawDebugCapsule(GetWorld(), TargetLocation, CapsuleHalfHeight, CapsuleRadius,
			                 FQuat::Identity, FColor::Orange, false, DashDuration + 1.0f, 0, 3.0f);
		}
	}

	// 대쉬 시작 이펙트
	FGameplayCueParameters StartCueParams;
	StartCueParams.Location = StartLocation;
	StartCueParams.SourceObject = Character;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
		FOmochaGameplayTags::Get().GameplayCue_Movement_Dash,
		StartCueParams
	);

	// AbilityTask_MoveToLocation 사용
	UAbilityTask_MoveToLocation* MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(
		this,
		NAME_None,
		TargetLocation,
		DashDuration,
		DashCurve,
		nullptr
	);

	if (MoveTask)
	{
		// 델리게이트 바인딩 (OnTargetLocationReached만 있음)
		MoveTask->OnTargetLocationReached.AddDynamic(this, &UDashSkillAbility::OnDashCompleted);

		// 태스크 활성화
		MoveTask->ReadyForActivation();

		UE_LOG(LogTemp, Log, TEXT("Dash task started from %s to %s"),
		       *StartLocation.ToString(), *TargetLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create MoveToLocation task"));
	}
}

void UDashSkillAbility::OnDashCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("Dash completed via AbilityTask"));

	// 대쉬 완료 이펙트
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		FGameplayCueParameters EndCueParams;
		EndCueParams.Location = Character->GetActorLocation();
		EndCueParams.SourceObject = Character;

		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
			FOmochaGameplayTags::Get().GameplayCue_Movement_Dash,
			EndCueParams
		);
	}
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UDashSkillAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                   bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
