// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaGameplayAbility.h"
#include "OmochaReloadAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaReloadAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	UOmochaReloadAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags =nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnReloadFinished();
};
