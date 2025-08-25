// Fill out your copyright notice in the Description page of Project Settings.
#include "AbilitySystem/Abilities/OmochaOmegaAbility.h"

#include "OmochaGameplayTags.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaCharacterConfigComponent.h"
#include "GameFramework/Character.h"
#include "Player/OmochaPlayerController.h"

UOmochaOmegaAbility::UOmochaOmegaAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOmochaOmegaAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
    
	// ConfigComponent 사용
	UOmochaCharacterConfigComponent* ConfigComp = Character->FindComponentByClass<UOmochaCharacterConfigComponent>();
	if (!ConfigComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ConfigComponent를 찾을 수 없습니다!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
    
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (bIsToggleMode)
		{
			// 오메가 상태 일 때 변신해제, 일반 상태 일 때, 변신 
			if (ConfigComp->IsInOmegaState())
			{
				UE_LOG(LogTemp, Warning, TEXT("변신해제"));
				ConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Normal);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("변신"));
				ConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Omega);
			}
		}
		else
		{
			// 일반 방식: 무조건 변신
			UE_LOG(LogTemp, Warning, TEXT("변신"));
			ConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Omega);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UOmochaOmegaAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}
