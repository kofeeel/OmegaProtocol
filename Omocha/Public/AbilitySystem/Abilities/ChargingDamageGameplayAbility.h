// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "ChargingDamageGameplayAbility.generated.h"

class AChargingArea;
/**
 * 
 */
UCLASS()
class OMOCHA_API UChargingDamageGameplayAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AChargingArea* SpawnArea(const FVector& Location);

	UFUNCTION(BlueprintCallable)
	void Shock(const TArray<AActor*> Enemies);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChargingArea> AreaClass;
};
