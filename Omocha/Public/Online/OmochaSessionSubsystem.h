// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OmochaSessionSubsystem.generated.h"

/**
 * 
 */


class CSteamID;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnFunctionSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionCompelte, EOnJoinSessionCompleteResult::Type Result);

UCLASS()
class OMOCHA_API UOmochaSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UOmochaSessionSubsystem();
	
	void InitSubsystem();

	void CreateSession(int32 NumPublicConnections, FString RoomName);

	void FindSessions(int32 MaxSearchResult, bool bSearch = false);
	
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	void DestroySession();

	void StartSession();

	static UTexture2D* GetSteamAvatar(uint64 InSteamID);

	FMultiplayerOnFunctionSessionComplete OnMultiplayerCreateSessionComplete;
	
	FMultiplayerOnFindSessionComplete OnMultiplayerFindSessionComplete;
	
	FMultiplayerOnFindSessionComplete OnMultiplayerSearchSessionComplete;
	
	FMultiplayerOnJoinSessionCompelte OnMultiplayerJoinSessionComplete;
	
	FMultiplayerOnFunctionSessionComplete OnMultiplayerDestroySessionComplete;
	
	FMultiplayerOnFunctionSessionComplete OnMultiplayerStartSessionComplete;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnFindSessionsComplete(bool bWasSuccessful);
	
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	
private:
	IOnlineSessionPtr SessionInterface;
	
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionDestroy = false;

	bool bSearchAndJoin = false;

	int32 LastNumPublicConnections = 0;

	FString LastRoomName = FString(); 
};
