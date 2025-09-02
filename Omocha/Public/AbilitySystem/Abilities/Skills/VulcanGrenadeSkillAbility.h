// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "VulcanGrenadeSkillAbility.generated.h"

class UOmochaMouseHitTask;
class AOmochaProjectile;
/**
 * 
 */
UCLASS()
class OMOCHA_API UVulcanGrenadeSkillAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()
	
public:
	UVulcanGrenadeSkillAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Projectile")
	bool bIsHowitzer = true;
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

	virtual void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

private:
	virtual void FireProjectileAtTarget(const FVector& TargetLocation) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Trajectory")
	float ProjectileGravityScale = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Trajectory")
	float DesiredLaunchSpeed = 100.0f;

private:
	void SpawnProjectileWithVelocity(const FVector& LaunchLocation, const FVector& LaunchVelocity);

	void SpawnProjectile(const FVector& LaunchLocation, const FRotator& LaunchRotation);
};
	