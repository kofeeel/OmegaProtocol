// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "OmochaHitReactAbility.generated.h"

/**
 * 
 */
class UAnimMontage;
class USoundBase;
class UNiagaraSystem;
class UMaterialInterface;

UCLASS()
class OMOCHA_API UOmochaHitReactAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	UOmochaHitReactAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayAbilityActivationInfo ActivationInfo,
	   const FGameplayEventData* EventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayAbilityActivationInfo ActivationInfo,
	   bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings")
	float HitReactDuration = 0.5f;

private:
	FTimerHandle HitReactTimer;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	void OnTimerFinished();
};