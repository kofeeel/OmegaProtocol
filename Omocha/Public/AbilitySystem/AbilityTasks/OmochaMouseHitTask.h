// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "OmochaMouseHitTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&,
                                            DataHandle);

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaMouseHitTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (
		DisplayName = "TargetDataUnderMouse",
		HidePin = "OwningAbility",
		DefaultToSelf = "OwningAbility",
		BluerpintInternalUseOnly = "true"))
	static UOmochaMouseHitTask* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (
		DisplayName = "TargetDataUnderMouseVisibility",
		HidePin = "OwningAbility",
		DefaultToSelf = "OwningAbility",
		BlueprintInternalUseOnly = "true"))
	static UOmochaMouseHitTask* CreateTargetDataUnderMouseVisibility(UGameplayAbility* OwningAbility);


	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

private:
	virtual void Activate() override;

	void SendMouseCursorData();
	void SendMouseCursorDataVisibility();

	void OnHitDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	                                 FGameplayTag ActivationTag) const;

	bool bUseVisibilityTrace = false;
};
