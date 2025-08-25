// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Skills/BaseInteractionGameplayAbility.h"
#include "PickupInteractionGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UPickupInteractionGameplayAbility : public UBaseInteractionGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
