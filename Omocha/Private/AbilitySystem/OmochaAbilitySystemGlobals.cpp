// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAbilitySystemGlobals.h"

#include "AbilitySystem/OmochaAbilityTypes.h"

FGameplayEffectContext* UOmochaAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FOmochaGameplayEffectContext();
}
