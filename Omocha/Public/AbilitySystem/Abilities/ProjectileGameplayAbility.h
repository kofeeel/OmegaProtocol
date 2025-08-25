// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageGameplayAbility.h"
#include "ProjectileGameplayAbility.generated.h"

class AOmochaProjectile;
class ACharacter;
/**
 * 
 */
UCLASS()
class OMOCHA_API UProjectileGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& TargetLocation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AOmochaProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<USoundBase> FireSound;

	virtual void FireProjectileAtTarget(const FVector& TargetLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void StartAttackSequence(const FVector& TargetLocation);

	void PlayMuzzleEffects(const FVector& Location, const FVector& Direction);

	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void OnMuzzleEffectTriggered(const FVector& Location, const FVector& Direction);

	UPROPERTY(BlueprintReadWrite, Category = "Projectile|Lifecycle")
	float MaxRange = 0.f;
};
