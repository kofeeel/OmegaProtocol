// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "AbilitySystem/LightningBowTypes.h"
#include "BowLightningFireAbility.generated.h"

class UOmochaMouseHitTask;

UCLASS()
class OMOCHA_API UBowLightningFireAbility : public UProjectileGameplayAbility
{
	GENERATED_BODY()

public:
	UBowLightningFireAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lightening Bow")
	TArray<FBowLightningChargeData> ChargeLevelData;

	//UPROPERTY()
	//TObjectPtr<UOmochaMouseHitTask> MouseHit = nullptr;
	
private:
	int32 GetCurrentChargeLevel() const;
	void SpawnProjectileWithChargeLevel(int32 ChargeLevel);
	void FireProjectileWithDamageMultiplier(TSubclassOf<AOmochaProjectile> InProjectileClass, float DamageMultiplier);
};
