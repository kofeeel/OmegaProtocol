// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "OmochaPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bCanBeOverlap = false;

	UPROPERTY(EditAnywhere)
	bool bIsSpawned = false;
};
