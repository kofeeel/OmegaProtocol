// Copyright It's My Turn


#include "Online/OmochaSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystemUtils.h"
#include "ThirdParty/Steamworks/Steamv157/sdk/public/steam/steam_api.h" 

UOmochaSessionSubsystem::UOmochaSessionSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	
}

void UOmochaSessionSubsystem::InitSubsystem()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UOmochaSessionSubsystem::CreateSession(int32 NumPublicConnections, FString RoomName)
{
	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		bCreateSessionDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastRoomName = RoomName;
		return DestroySession();
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("GUID"), FGuid::NewGuid().ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(FName("RoomName"), RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);
}

void UOmochaSessionSubsystem::FindSessions(int32 MaxSearchResult, bool bSearch)
{
	if (!SessionInterface.IsValid()) return;

	if (bSearch) bSearchAndJoin = true;
		
	FindSessionCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResult;
	LastSessionSearch->bIsLanQuery = Online::GetSubsystem(GetWorld())->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef());
}


void UOmochaSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		OnMultiplayerJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult);
}

void UOmochaSessionSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		OnMultiplayerDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	SessionInterface->DestroySession(NAME_GameSession);
}

void UOmochaSessionSubsystem::StartSession()
{
	
}

UTexture2D* UOmochaSessionSubsystem::GetSteamAvatar(uint64 InSteamID)
{
	const CSteamID SteamID(InSteamID);
	if (!SteamAPI_Init())
	{
		return nullptr;
	}

	int AvatarHandle = SteamFriends()->GetMediumFriendAvatar(SteamID);

	if (AvatarHandle == 0)
	{
		return nullptr;
	}

	uint32 Width, Height;
	if (!SteamUtils()->GetImageSize(AvatarHandle, &Width, &Height))
	{
		return nullptr;
	}

	TArray<uint8> AvatarRGBA;
	AvatarRGBA.SetNum(Width * Height * 4);

	if (!SteamUtils()->GetImageRGBA(AvatarHandle, AvatarRGBA.GetData(), AvatarRGBA.Num()))
	{
		return nullptr;
	}

	UTexture2D* AvatarTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
	if (!AvatarTexture)
	{
		return nullptr;
	}

	void* TextureData = AvatarTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, AvatarRGBA.GetData(), AvatarRGBA.Num());
	AvatarTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	AvatarTexture->UpdateResource();

	return AvatarTexture;
}

void UOmochaSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnMultiplayerCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UOmochaSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		OnMultiplayerFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	for (auto& Result: LastSessionSearch->SearchResults)
	{
		Result.Session.SessionSettings.bUseLobbiesIfAvailable = Result.Session.SessionSettings.bUsesPresence;
	}

	if (bSearchAndJoin)
	{
		bSearchAndJoin = false;
		OnMultiplayerSearchSessionComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);	
	}
	else
	{
		OnMultiplayerFindSessionComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);	
	}
}

void UOmochaSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	OnMultiplayerJoinSessionComplete.Broadcast(Result);
}

void UOmochaSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	OnMultiplayerDestroySessionComplete.Broadcast(bWasSuccessful);
	if (bWasSuccessful && bCreateSessionDestroy)
	{
		bCreateSessionDestroy = false;
		CreateSession(LastNumPublicConnections, LastRoomName);
	}
}

void UOmochaSessionSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
