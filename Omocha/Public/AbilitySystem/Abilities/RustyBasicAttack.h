// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "TraceGameplayAbility.h"
#include "RustyBasicAttack.generated.h"

class UOmochaMouseHitTask;
/**
 * 
 */
UCLASS()
class OMOCHA_API URustyBasicAttack : public UTraceGameplayAbility
{
	GENERATED_BODY()

public:
	URustyBasicAttack();

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

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void OnInputReleased();

	UFUNCTION(BlueprintCallable, Category = "Target Data")
	void RequestTargetData();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void StartAttackSequence(const FVector& TargetLocation);

private:
	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UPROPERTY()
	TObjectPtr<UOmochaMouseHitTask> CurrentTargetDataTask;

	UPROPERTY()
	bool bHasProcessedTargetData = false;
};
