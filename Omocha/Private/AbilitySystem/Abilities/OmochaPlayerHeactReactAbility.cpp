// Copyright It's My Turn


#include "AbilitySystem/Abilities/OmochaPlayerHeactReactAbility.h"

#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"

UOmochaPlayerHeactReactAbility::UOmochaPlayerHeactReactAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Effects_HitReact);
	SetAssetTags(Tags);

	AbilityTags.AddTag(FOmochaGameplayTags::Get().Effects_HitReact);
}

void UOmochaPlayerHeactReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Player HitReact Active Ability"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ApplyHitReactEffects();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(HitReactTimer,
											   [this, Handle, ActorInfo, ActivationInfo]()
											   {
												   EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
											   },
											   HitReactDuration, false);
	}
}

void UOmochaPlayerHeactReactAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveHitReactEffects();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitReactTimer);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOmochaPlayerHeactReactAbility::ApplyHitReactEffects()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(FOmochaGameplayTags::Get().Effects_HitReact);
	}

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ClientSetCameraFade(true, 
		   FColor(255, 0, 0, 128), 
		   FVector2D(0.6f, 1.0f), 
		   HitReactDuration);
	}
}

void UOmochaPlayerHeactReactAbility::RemoveHitReactEffects()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(FOmochaGameplayTags::Get().Effects_HitReact);
	}

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ClientSetCameraFade(false, FColor::White, 
			FVector2D(0.0f, 0.0f), 0.1f);
	}
}
