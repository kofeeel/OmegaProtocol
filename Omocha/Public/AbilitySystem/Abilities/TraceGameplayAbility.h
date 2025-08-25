#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "TraceGameplayAbility.generated.h"

class UOmochaMouseHitTask;

namespace EDrawDebugTrace
{
	enum Type : int;
}

UCLASS()
class OMOCHA_API UTraceGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UTraceGameplayAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void ExecuteOverlapAndApplyDamage(const FVector& TargetLocation, float OverlapRadius = 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overlap Settings")
	float OverlapStartForwardOffset = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overlap Settings|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity Pull")
	bool bShouldApplyGravityPull = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity Pull",
		meta = (EditCondition = "bShouldApplyGravityPull"))
	float GravityPullForce = 3000.f;

private:
	float DrawDebugDuration = 2.0f;
	bool PerformOverlap(TArray<FHitResult>& OutHitResults, const FVector& TargetLocation, float OverlapRadius);
	void ProcessHitResults(const TArray<FHitResult>& HitResults);
	void ApplyGravityPull(const TArray<FHitResult>& HitResults);

	FVector CurrentTraceCenter;
};
