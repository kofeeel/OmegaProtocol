// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ChainLightning.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChainLightningTargetsDeterminedSignature, const TArray<AActor*>&,
                                            TargetChain);

UCLASS()
class OMOCHA_API UAbilityTask_ChainLightning : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FChainLightningTargetsDeterminedSignature OnTargetsDetermined;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_ChainLightning* CreateChainLightningTask(
		UGameplayAbility* OwningAbility,
		AActor* InitialTarget,
		int32 InMaxChainCount,
		float InSearchRadius
	);

	virtual void Activate() override;

private:
	void FindNextTarget(AActor* CurrentTarget);

	UPROPERTY()
	TObjectPtr<AActor> InitialTarget;

	UPROPERTY()
	int32 MaxChainCount;

	UPROPERTY()
	float SearchRadius;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> TargetChain;
};
