#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "OmegaGiveAbility.generated.h"

class UOmochaMouseHitTask;

UCLASS()
class OMOCHA_API UOmegaGiveAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	UOmegaGiveAbility();

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
	void StartGiveSequence(const FVector& TargetLocation, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Transform")
	void SpawnGiveProjectile(AActor* TargetActor);

private:
	bool CheckActivationConditions() const;
	void RequestGiveTargetData();

	UFUNCTION()
	void OnGiveTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	bool IsValidGiveTarget(AActor* TargetActor) const;

	UPROPERTY()
	UOmochaMouseHitTask* CurrentTargetDataTask = nullptr;

	bool bHasProcessedTargetData = false;
};
