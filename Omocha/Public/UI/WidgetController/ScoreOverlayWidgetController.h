// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OverlayWidgetController.h"
#include "ScoreOverlayWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OMOCHA_API UScoreOverlayWidgetController : public UOverlayWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadCastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
};
