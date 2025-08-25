// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaUserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineWidget.generated.h"

class URoomListWidget;
class URoomWidget;
class UOmochaSessionSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIsSessionSuccessful, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class OMOCHA_API UOnlineWidget : public UOmochaUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitOnlineWidget(
		int32 NumberOfPublicConnections = 100,	FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")));

	UPROPERTY(BlueprintAssignable)
	FIsSessionSuccessful OnFindSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FIsSessionSuccessful OnSearchSessionComplete;
	
	UPROPERTY(BlueprintAssignable)
	FIsSessionSuccessful OnCreateSessionComplete;

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSession(const TArray<FOnlineSessionSearchResult>& InSessionResults, bool bWasSuccessful);
	
	void OnSearchSession(const TArray<FOnlineSessionSearchResult>& InSessionResults, bool bWasSuccessful);

	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URoomListWidget> RoomListWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> RoomListWidgetClass;
	
private:
	UFUNCTION(BlueprintCallable)
	void CreateButtonClicked(FString RoomName);
	
	UFUNCTION(BlueprintCallable)
	void JoinButtonClicked();

	UFUNCTION(BlueprintCallable)
	void SettingButtonClicked();

	UFUNCTION(BlueprintCallable)
	void EnterButtonClicked(FString Guid, FString RoomName);
	
	void MenuTearDown();

	void PrintErrorMessage(FString Message);

	TObjectPtr<UOmochaSessionSubsystem> SessionSubsystem;

	int32 NumPublicConnections = 100;

	FString PathToLobby = FString(TEXT("/Game/Maps/Lobby"));

	bool bSearch = false;

	FString SearchRoomId;

	FString SearchRoomName;
};
