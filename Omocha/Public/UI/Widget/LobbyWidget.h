// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaUserWidget.h"
#include "Game/OmochaLobbyGameMode.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "LobbyWidget.generated.h"

class AOmochaPlayerState;
class AOmochaPlayerController;
enum class ECharacterType : uint8;
class UButton;
/**
 * 
 */
UCLASS()
class OMOCHA_API ULobbyWidget : public UOmochaUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void BindingButtons(AOmochaPlayerController* PlayerController, AOmochaPlayerState* PlayerState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NextLevel = FString();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MainMenuWorld = FName("/Game/Maps/Start");

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartOrStopTimer(bool bAllReady);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeForTravel = 5.f;

	UFUNCTION(BlueprintCallable)
	void ClickOnLeave();

	UFUNCTION(BlueprintCallable)
	virtual void ClickOnReady();

	UFUNCTION(BlueprintCallable)
	void ClickOnRusty();

	UFUNCTION(BlueprintCallable)
	void ClickOnVulcan();

	UFUNCTION(BlueprintCallable)
	void ClickOnZenith();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bReady = false;
	
private:
	FString PlayerId;
	ECharacterType CharacterType = ECharacterType::None;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	
	void ChooseCharacter(ECharacterType Type);
	
};
