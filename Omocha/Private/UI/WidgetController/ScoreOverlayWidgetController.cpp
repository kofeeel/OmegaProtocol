// Copyright It's My Turn


#include "UI/WidgetController/ScoreOverlayWidgetController.h"
#include "Player/OmochaPlayerController.h"

void UScoreOverlayWidgetController::BroadCastInitialValues()
{
	if (PlayerController)
	{
		PlayerController->Server_ScoreDataRequest();
	}
}

void UScoreOverlayWidgetController::BindCallbacksToDependencies()
{
	
}
