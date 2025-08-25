// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileGameplayAbility.h"
#include "BasicAttackGameplayAbility.generated.h"

class UOmochaMouseHitTask;
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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attack State")
	bool bShouldEndAttack = false;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SetShouldEndAttack(bool bValue) { bShouldEndAttack = bValue; }

	UFUNCTION(BlueprintPure, Category = "Attack")
	bool GetShouldEndAttack() const { return bShouldEndAttack; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Attacok")
	void OnInputReleased();

	// 마우스 위치를 가져오는 Task를 요청
	UFUNCTION(BlueprintCallable, Category = "Target Data")
	void RequestTargetData();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void SettingMaxRange();

private:
	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 상태 변수들
	UPROPERTY()
	TObjectPtr<UOmochaMouseHitTask> CurrentTargetDataTask;

	UPROPERTY()
	bool bHasProcessedTargetData = false;
};
