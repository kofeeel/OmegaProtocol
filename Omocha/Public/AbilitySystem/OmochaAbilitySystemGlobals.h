// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"
#include "OmochaAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OMOCHA_API UOmochaAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Debuff")
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> DebuffEffectMap;
};
