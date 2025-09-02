// Copyright It's My Turn


#include "AbilitySystem/Abilities/PlayerInteractAbility.h"

#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "Interaction/OmochaInteractionInterface.h"

UPlayerInteractAbility::UPlayerInteractAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Interact;
}

void UPlayerInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthority(&ActivationInfo)) {
		return;
	}

	UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ASC) {
		AActor* InteractableActor = ASC->CurrentInteractable.Get();
		if (InteractableActor && InteractableActor->Implements<UOmochaInteractionInterface>()) {
			IOmochaInteractionInterface::Execute_ExecuteInteraction(InteractableActor, ActorInfo->AvatarActor.Get());
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
