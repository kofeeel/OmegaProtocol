// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "LobbyWidget.h"
#include "ScoreBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UScoreBoardWidget : public ULobbyWidget
{
	GENERATED_BODY()

public:
	virtual void ClickOnReady() override;

};
