// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "LightningBowTypes.generated.h"

USTRUCT(BlueprintType)
struct FBowLightningChargeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AOmochaProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier = 1.f;

	FBowLightningChargeData()
	{
		ProjectileClass = nullptr;
		DamageMultiplier = 1.f;
	}
};