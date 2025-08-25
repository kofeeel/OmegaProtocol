// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "OmochaEnemyTestAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaEnemyTestAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UOmochaEnemyTestAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy Attack")
	void ApplyDamageToNearbyPlayers();
	
private:
	UPROPERTY()
	bool bHasProcessedAttack = false;
};
