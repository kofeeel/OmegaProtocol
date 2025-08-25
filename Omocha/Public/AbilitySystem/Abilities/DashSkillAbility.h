#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MoveGameplayAbility.h"
#include "DashSkillAbility.generated.h"

class UCurveFloat;

/**
 * 대쉬 스킬 (부드러운 이동 타입)
 */
UCLASS()
class OMOCHA_API UDashSkillAbility : public UMoveGameplayAbility
{
	GENERATED_BODY()

public:
	UDashSkillAbility();

protected:
	virtual void ExecuteMovement(const FVector& TargetLocation) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
							const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo,
							bool bReplicateEndAbility,
							bool bWasCancelled) override;

	// 대쉬 관련 파라미터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashDuration = 0.3f;  // 대쉬 지속 시간

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	UCurveFloat* DashCurve;  // 대쉬 속도 커브

	// 대쉬 완료 콜백
	UFUNCTION()
	void OnDashCompleted();
};