#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MoveGameplayAbility.h"
#include "TeleportSkillAbility.generated.h"

/**
 * 즉시 이동 스킬 (플래시 타입)
 */
UCLASS()
class OMOCHA_API UTeleportSkillAbility : public UMoveGameplayAbility
{
	GENERATED_BODY()

public:
	UTeleportSkillAbility();

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
	
	virtual void ExecuteMovement(const FVector& TargetLocation) override;

	// 텔레포트 관련 파라미터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teleport")
	float TeleportDelay = 0.1f;  // 텔레포트 전 딜레이

private:
	void PerformTeleport(const FVector& TargetLocation);
};