// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillGameplayAbility.h"
#include "MoveGameplayAbility.generated.h"

namespace EDrawDebugTrace
{
	enum Type : int;
}

class UOmochaMouseHitTask;
/**
 * 
 */
UCLASS()
class UMoveGameplayAbility : public USkillGameplayAbility
{
	GENERATED_BODY()

public:
	UMoveGameplayAbility();
	// === 오버라이드할 가상 함수들 ===
	UFUNCTION(BlueprintCallable, Category = "Move")
	virtual void ExecuteMovement(const FVector& TargetLocation)
	{
	};

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
	// C++에서만 쓸 내부 함수들
	UFUNCTION(BlueprintCallable, Category = "Move")
	FVector GetValidMoveLocation(const FVector& DesiredLocation) const;

	bool IsLocationValid(const FVector& Location) const;
	float CalculateActualDistance(const FVector& TargetLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Move")
	FVector FindGroundLocation(const FVector& Location) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overlap Settings|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;
};
