// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "BlockDashAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UBlockDashAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UFUNCTION(BlueprintCallable, Category = "BlockDash Ability")
	bool CommitAndConsumeCharge();
	
	UFUNCTION()
	void OnChargeCooldownEnd(float TimeRemaining);

	UPROPERTY(EditDefaultsOnly, Category = "BlockDash Ability")
	TSubclassOf<UGameplayEffect> ConsumeChargeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "BlockDash Ability")
	TSubclassOf<UGameplayEffect> AddChargeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "BlockDash Ability")
	TSubclassOf<UGameplayEffect> ChargeRegenCooldownEffect;
};
