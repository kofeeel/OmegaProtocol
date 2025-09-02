// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "TraceGameplayAbility.h"
#include "RustyBasicAttack.generated.h"

class UOmochaMouseHitTask;
/**
 * 
 */
UCLASS()
class OMOCHA_API URustyBasicAttack : public UTraceGameplayAbility
{
	GENERATED_BODY()

public:
	URustyBasicAttack();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
	void StartAttackSequence(const FVector& TargetLocation);

	virtual void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
};
