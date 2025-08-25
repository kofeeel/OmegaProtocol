// Copyright It's My Turn


#include "UI/Widget/ScoreBoardWidget.h"
#include "Player/OmochaPlayerController.h"

void UScoreBoardWidget::ClickOnReady()
{
	if (bReady)
	{
		bReady = false;
	}
	else
	{
		bReady = true;
	}
		
	if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(GetOwningPlayer()))
	{
		PlayerController->Server_PlayerRestartReady(bReady);
	}
}
