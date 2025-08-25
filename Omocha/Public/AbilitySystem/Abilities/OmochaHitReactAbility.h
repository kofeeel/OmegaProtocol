// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "OmochaHitReactAbility.generated.h"



UCLASS()
class OMOCHA_API UOmochaHitReactAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	UOmochaHitReactAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayAbilityActivationInfo ActivationInfo,
	   const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayAbilityActivationInfo ActivationInfo,
	   bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings | Effects")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings | Effects")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings | Effects")
	TObjectPtr<UNiagaraSystem> HitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings | Critical Hit")
	TObjectPtr<UAnimMontage> CriticalHitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings | Critical Hit")
	TObjectPtr<USoundBase> CriticalHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings | Critical Hit")
	TObjectPtr<UNiagaraSystem> CriticalHitParticle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings")
	float HitReactDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings")
	bool bApplyColorEffect = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitReact Settings", 
			meta = (EditCondition = "bApplyColorEffect", DisplayName = "Hit Flash Material"))
	TObjectPtr<UMaterialInterface> HitFlashMaterial;

    
private:
	FTimerHandle HitReactTimer;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	UFUNCTION()
	void OnMontageEnded();

	void OnTimerFinished();
	void ApplyHitReactEffects(USoundBase* Sound, UNiagaraSystem* Particle);
	void RemoveHitReactEffects();
};