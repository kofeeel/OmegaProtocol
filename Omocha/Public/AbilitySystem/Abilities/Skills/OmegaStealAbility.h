#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "OmegaStealAbility.generated.h"

class UOmochaMouseHitTask;

UCLASS()
class OMOCHA_API UOmegaStealAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	UOmegaStealAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Transform Projectile")
	TSubclassOf<class AOmochaTransformProjectile> TransformProjectileClass;

	UFUNCTION(BlueprintImplementableEvent, Category = "Transform")
	void StartStealSequence(const FVector& TargetLocation, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Transform")
	void SpawnStealProjectile(AActor* TargetActor);

private:
	bool CheckActivationConditions() const;
	void RequestTargetData();

	UFUNCTION()
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	bool IsValidStealTarget(AActor* TargetActor) const;

	UPROPERTY()
	UOmochaMouseHitTask* CurrentTargetDataTask = nullptr;

	bool bHasProcessedTargetData = false;
};
