// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OverlayWidgetController.h"
#include "LobbyOverlayWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OMOCHA_API ULobbyOverlayWidgetController : public UOverlayWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadCastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
};
