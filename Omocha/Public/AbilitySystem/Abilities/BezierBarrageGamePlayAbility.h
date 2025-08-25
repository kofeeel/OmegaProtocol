#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "BezierBarrageGamePlayAbility.generated.h"

UCLASS()
class OMOCHA_API UBezierBarrageGamePlayAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	UBezierBarrageGamePlayAbility();

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

	virtual void FireProjectileAtTarget(const FVector& TargetLocation) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Missile Barrage")
	int32 MissileCount = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage")
	float BarrageRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage")
	float MissileFlightDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage")
	float ControlPointHeight = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage")
	float ControlPointBackDistance = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage")
	float MissileSpawnDelay = 0.05f;

private:
	void SpawnMissileWithTrajectory(const FVector& StartPoint, const FVector& ControlPoint, const FVector& EndPoint);

	FVector CalculateControlPoint(const FVector& CharacterLocation, const FRotator& CharacterRotation,
	                              int32 MissileIndex) const;

	FVector GenerateRandomTargetLocation(const FVector& BaseTarget) const;

	UFUNCTION()
	void SpawnNextMissile();

	int32 CurrentMissileIndex = 0;
	FVector CachedTargetLocation = FVector::ZeroVector;
	FVector CachedCharacterLocation = FVector::ZeroVector;
	FRotator CachedCharacterRotation = FRotator::ZeroRotator;

	FTimerHandle MissileSpawnTimer;
	FTimerHandle ForceEndTimer;

	UPROPERTY()
	TArray<TObjectPtr<AOmochaProjectile>> SpawnedMissiles;
};
