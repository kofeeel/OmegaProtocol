// Copyright It's My Turn


#include "AbilitySystem/Abilities/Skills/BaseInteractionGameplayAbility.h"
#include "OmochaGameplayTags.h"

UBaseInteractionGameplayAbility::UBaseInteractionGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Interact;
}
