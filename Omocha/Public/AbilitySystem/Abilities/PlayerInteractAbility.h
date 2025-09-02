// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaGameplayAbility.h"
#include "PlayerInteractAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UPlayerInteractAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	UPlayerInteractAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
