// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "ChainLightningAbility.generated.h"

class AChainLightningVFXActor;
class UAbilityTask_ChainLightning;
class UNiagaraSystem;

/**
 * 
 * 
 */
UCLASS()
class OMOCHA_API UChainLightningAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UChainLightningAbility();

	UFUNCTION(BlueprintCallable, Category = "Chain Lightning")
	void ExecuteChainLightning(const FVector& TargetLocation);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	void StartChainLightningTask(AActor* InitialTarget);

	UFUNCTION()
	void OnChainTargetsDetermined(const TArray<AActor*>& TargetChain);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chain Lightning|Beam")
	TObjectPtr<UNiagaraSystem> BeamVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Chain Lightning|Config")
	int32 MaxChainCount = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chain Lightning|Config")
	float SearchRadius = 800.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chain Lightning|Config")
	float DamageFalloff = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chain Lightning|FX", meta = (Categories = "GameplayCue"))
	FGameplayTag ChainLightningCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TSubclassOf<AChainLightningVFXActor> VFXActorClass;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_ChainLightning> ChainLightningTask;
};
