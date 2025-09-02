#include "AbilitySystem/Abilities/OmochaHitReactAbility.h"
#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilitysystem/OmochaAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UOmochaHitReactAbility::UOmochaHitReactAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Effects_HitReact);
	SetAssetTags(Tags);
	
	AbilityTags.AddTag(FOmochaGameplayTags::Get().Effects_HitReact); 
}

void UOmochaHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* EventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TObjectPtr<UAbilitySystemComponent> ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) { return; }
	ASC->AddLooseGameplayTag(FOmochaGameplayTags::Get().Effects_HitReact);

	FGameplayCueParameters CueParams;
	if (EventData)
	{
		CueParams.EffectContext = EventData->ContextHandle;
		const bool bIsCritical = UOmochaAbilitySystemLibrary::IsCriticalHit(EventData->ContextHandle);
		CueParams.RawMagnitude = bIsCritical ? 1.f : 0.f;
	}

	ASC->ExecuteGameplayCue(FOmochaGameplayTags::Get().GameplayCue_HitReact, CueParams);
	
	GetWorld()->GetTimerManager().SetTimer(HitReactTimer, this, &UOmochaHitReactAbility::OnTimerFinished, HitReactDuration, false);
}

void UOmochaHitReactAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	TObjectPtr<UAbilitySystemComponent> ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(FOmochaGameplayTags::Get().Effects_HitReact);
	}
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitReactTimer);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOmochaHitReactAbility::OnTimerFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
