// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "OmochaAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitySpecsGiven, const TArray<FGameplayAbilitySpec>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FShieldAttributeChanged, const float);

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilitySpecsGiven AbilitiesSpecsGivenDelegate;
	FShieldAttributeChanged ShieldHealthChangedDelegate;
	FShieldAttributeChanged ShieldMaxHealthChangedDelegate;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities, const float Level);

	void AbilityActorInfoSet();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ActivateAbilityByTag(const FGameplayTag& Tag);

	void ForEachAbility(const FForEachAbility& Delegate);

	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector CachedSkillTargetLocation;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<AActor> CurrentInteractable;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float SkillScale = 1.f;

	UFUNCTION(Client, Reliable)
	void Client_BroadcastAbilityInfos(const TArray<FGameplayAbilitySpec>& AbilitySpecs);

	void BroadcastShieldHealthChanged(const FOnAttributeChangeData& Data) const;
	void BroadcastShieldMaxHealthChanged(const FOnAttributeChangeData& Data) const;

protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
	                         FActiveGameplayEffectHandle ActiveEffectHandle);

private:
	UFUNCTION(Client, Reliable)
	void Client_BroadcastShieldHealth(const float NewValue) const;

	UFUNCTION(Client, Reliable)
	void Client_BroadcastShieldMaxHealth(const float NewValue) const;
};
