// Copyright It's My Turn


#include "UI/Widget/LobbyWidget.h"

#include "OnlineSubsystem.h"
#include "Components/Button.h"
#include "Game/OmochaBaseGameMode.h"
#include "Game/OmochaGameInstance.h"
#include "Game/OmochaLobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OmochaPlayerController.h"
#include "Player/OmochaPlayerState.h"


void ULobbyWidget::BindingButtons(AOmochaPlayerController* PlayerController, AOmochaPlayerState* PlayerState)
{
	if (!PlayerController || !PlayerState) return;
	
	PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
	PlayerController->OnPrepareNextLevel.Clear();
	PlayerController->OnPrepareNextLevel.AddDynamic(this, &ThisClass::StartOrStopTimer);
	
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);

	if (AOmochaBaseGameMode* OmochaGM = Cast<AOmochaBaseGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		OmochaGM->SetNextMap(NextLevel);
	}	
}

void ULobbyWidget::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), MainMenuWorld, true);
	}
}

void ULobbyWidget::ClickOnLeave()
{
	if (bReady)
	{
		ClickOnReady();
	}
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
}

void ULobbyWidget::ClickOnReady()
{
	if (CharacterType == ECharacterType::None
		|| CharacterType == ECharacterType::NotJoined) return;
	
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
		PlayerController->Server_PlayerGetReady(PlayerId, bReady);
	}
}

void ULobbyWidget::ClickOnRusty()
{
	ChooseCharacter(ECharacterType::Rusty);
}

void ULobbyWidget::ClickOnVulcan()
{
	ChooseCharacter(ECharacterType::Vulcan);
}

void ULobbyWidget::ClickOnZenith()
{
	ChooseCharacter(ECharacterType::Zenith);
}

void ULobbyWidget::ChooseCharacter(ECharacterType Type)
{
	if (Type == CharacterType) return;
	CharacterType = Type;
	
	if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(GetOwningPlayer()))
	{
		PlayerController->Server_ChoosePlayerCharacter(PlayerId, Type);
	}
	if (bReady)
	{
		ClickOnReady();
	}
}
