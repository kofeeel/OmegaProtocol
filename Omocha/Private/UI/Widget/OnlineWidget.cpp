// Copyright It's My Turn


#include "UI/Widget/OnlineWidget.h"
#include "Online/OmochaSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "UI/HUD/OmochaHUD.h"
#include "UI/Widget/RoomListWidget.h"

void UOnlineWidget::InitOnlineWidget(int32 NumberOfPublicConnections, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		SessionSubsystem = GameInstance->GetSubsystem<UOmochaSessionSubsystem>();
	}

	if (SessionSubsystem)
	{
		SessionSubsystem->InitSubsystem();
		SessionSubsystem->OnMultiplayerCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		SessionSubsystem->OnMultiplayerFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
		SessionSubsystem->OnMultiplayerJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		SessionSubsystem->OnMultiplayerSearchSessionComplete.AddUObject(this, &ThisClass::OnSearchSession);
		SessionSubsystem->OnMultiplayerDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		SessionSubsystem->OnMultiplayerStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UOnlineWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	return false;
}

void UOnlineWidget::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UOnlineWidget::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		PrintErrorMessage(FString(TEXT("Failed created session")));
	}
	OnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UOnlineWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& InSessionResults, bool bWasSuccessful)
{
	if (!IsValid(SessionSubsystem)) return;
	if (RoomListWidget)
	{
		RoomListWidget->RemoveRoomWidgets();
		for (auto Result : InSessionResults)
		{
			RoomListWidget->CreateRoomWidget(Result);
		}
	}
	if (!bWasSuccessful)
	{
		PrintErrorMessage(FString(TEXT("Failed find session")));
	}
	else if (InSessionResults.Num() == 0)
	{
		PrintErrorMessage(FString(TEXT("Session not found")));
	}
	OnFindSessionComplete.Broadcast(bWasSuccessful);
}

void UOnlineWidget::OnSearchSession(const TArray<FOnlineSessionSearchResult>& InSessionResults, bool bWasSuccessful)
{
	if (!IsValid(SessionSubsystem) || !bSearch || !bWasSuccessful || InSessionResults.Num() == 0)
	{
		PrintErrorMessage(FString(TEXT("Failed Search session")));
	}

	bSearch = false;
	for (auto Result : InSessionResults)
	{
		FString RoomId, RoomName;
		Result.Session.SessionSettings.Get(FName("GUID"), RoomId);
		Result.Session.SessionSettings.Get(FName("RoomName"), RoomName);
		if (RoomId.Equals(SearchRoomId) && RoomName.Equals(SearchRoomName))
		{
			SearchRoomId.Empty();
			SearchRoomName.Empty();
			SessionSubsystem->JoinSession(Result);
			return;
		}
	}
	SearchRoomId.Empty();
	SearchRoomName.Empty();

	OnSearchSessionComplete.Broadcast(bWasSuccessful);
}

void UOnlineWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UOnlineWidget::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		PrintErrorMessage(FString(TEXT("Failed session destroyed")));
	}
}

void UOnlineWidget::OnStartSession(bool bWasSuccessful)
{
}

void UOnlineWidget::CreateButtonClicked(FString RoomName)
{
	if (SessionSubsystem)
	{
		if (RoomName.IsEmpty())
		{
			RoomName = FString(TEXT("Shit Game"));
		}
		SessionSubsystem->CreateSession(NumPublicConnections, RoomName);
	}
}

void UOnlineWidget::JoinButtonClicked()
{
	if (SessionSubsystem)
	{
		SessionSubsystem->FindSessions(10000);
	}
}

void UOnlineWidget::SettingButtonClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AOmochaHUD* HUD = Cast<AOmochaHUD>(PC->GetHUD()))
		{
			HUD->ShowSettingsWidget();
		}
	}
}

void UOnlineWidget::EnterButtonClicked(FString Guid, FString RoomName)
{
	if (SessionSubsystem && RoomListWidget)
	{
		bSearch = true;
		SearchRoomId = Guid;
		SearchRoomName = RoomName;
		SessionSubsystem->FindSessions(10000, true);
	}
}

void UOnlineWidget::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

void UOnlineWidget::PrintErrorMessage(FString Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Red,
			Message);
	}
}
