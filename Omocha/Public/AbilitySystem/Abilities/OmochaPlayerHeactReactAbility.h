// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "OmochaPlayerHeactReactAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaPlayerHeactReactAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()
public:
	UOmochaPlayerHeactReactAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings")
	float HitReactDuration = 0.8f;

private:
	FTimerHandle HitReactTimer;
	
	void ApplyHitReactEffects();
	void RemoveHitReactEffects();
	
};
