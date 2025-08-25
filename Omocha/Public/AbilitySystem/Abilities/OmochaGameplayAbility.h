// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OmochaGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag InputGameplayTag;

	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	float CooldownDuration = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	FGameplayTag AbilityCooldownTag;

protected:
	float GetCooldown(float InLevel = 1.f) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Settings")
	bool bBlockedWhenDead = true;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnBlueprintCommit();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnPlaySFX();
};
