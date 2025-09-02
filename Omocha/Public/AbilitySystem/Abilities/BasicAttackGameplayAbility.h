// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileGameplayAbility.h"
#include "BasicAttackGameplayAbility.generated.h"

class AOmochaProjectile;
/**
 * 
 */
UCLASS()
class OMOCHA_API UBasicAttackGameplayAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	UBasicAttackGameplayAbility();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void SettingMaxRange();

	virtual void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
};
