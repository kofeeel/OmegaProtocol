// Copyright It's My Turn


#include "UI/WidgetController/LobbyOverlayWidgetController.h"
#include "Player/OmochaPlayerController.h"

void ULobbyOverlayWidgetController::BroadCastInitialValues()
{
	if (PlayerController)
	{
		PlayerController->Server_InitializeRequest();
	}
}

void ULobbyOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!IsValid(PlayerController)) return;
	
	PlayerController->OnChatMessageArrived.AddUObject(this, &ULobbyOverlayWidgetController::ChatMessageArrived);
}
