// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "RustyHookAbility.generated.h"

class AHook;
/**
 * 
 */
UCLASS()
class OMOCHA_API URustyHookAbility : public UDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AHook* SpawnHook(const FVector& HitLocation);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Hook")
	float HookMaxLength = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Hook")
	float Padding = 30.f;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHook> HookClass;
};
