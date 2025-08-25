// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChargingArea.generated.h"

UCLASS()
class OMOCHA_API AChargingArea : public AActor
{
	GENERATED_BODY()

public:
	AChargingArea();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector2D GetAreaSize();

	UPROPERTY(BlueprintReadWrite)
	FVector Direction = FVector::ZeroVector;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Charging")
	bool bUpdateDirection = false;
};
