// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "OmochaOmegaAbility.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaOmegaAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	UOmochaOmegaAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Settings")
	bool bIsToggleMode = true;

	// 변신 중 다른 어빌리티 사용 차단
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Settings")
	bool bBlockOtherAbilities = false;

private:
	
};
