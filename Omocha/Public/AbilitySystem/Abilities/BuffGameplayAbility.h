// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SkillGameplayAbility.h"
#include "BuffGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UBuffGameplayAbility : public USkillGameplayAbility
{
	GENERATED_BODY()

public:
	UBuffGameplayAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	UFUNCTION(BlueprintCallable, Category = "BuffGameplayAbility")
	void GenerateBuff();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
	TSubclassOf<UGameplayEffect> BuffEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Buff")
	bool bIsAreaOfEffect = false;
};
