// Copyright It's My Turn


#include "AbilitySystem/Abilities/PickupInteractionGameplayAbility.h"

#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "Interaction/OmochaInteractionInterface.h"

void UPickupInteractionGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo,
                                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthority(&ActivationInfo))
	{
		return;
	}
	
	UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!ASC) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* PickupActor = ASC->CurrentInteractable.Get();

	if (PickupActor && PickupActor->Implements<UOmochaInteractionInterface>()) {
		IOmochaInteractionInterface::Execute_ExecuteInteraction(PickupActor, ActorInfo->AvatarActor.Get());
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
