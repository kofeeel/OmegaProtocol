// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "EnemyRangeBasicAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UEnemyRangeBasicAttackAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	UEnemyRangeBasicAttackAbility();

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

private:
	void GetPlayerLocation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Settings", meta = (AllowPrivateAccess = "true"))
	float MaxAttackRange = 1500.0f;
};
