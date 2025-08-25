// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SkillGameplayAbility.h"
#include "AbilitySystem/LightningBowTypes.h"
#include "BowLightningChargeAbility.generated.h"

	
UCLASS()
class OMOCHA_API UBowLightningChargeAbility : public USkillGameplayAbility
{
	GENERATED_BODY()

public:
	UBowLightningChargeAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow Lightning")
	TArray<FBowLightningChargeData> ChargeLevelData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow Lightning")
	float MaxChargeTime = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bow Lightning")
	TSubclassOf<UGameplayEffect> ChargeEffect;
	
private:
	bool bAbilityEnded = false;
	FTimerHandle ChargeLevelTimer;
	FTimerHandle AutoFireTimer;
	FActiveGameplayEffectHandle ChargeEffectHandle;

	void UpdateChargeLevel();
	void FireProjectile();
	void ApplyChargeEffect(int32 Level);
};
