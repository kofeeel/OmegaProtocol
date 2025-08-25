// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaBaseGameMode.h"
#include "OmochaLobbyGameMode.generated.h"

class AOmochaPlayerCharacter;
class AOmochaLobbyCharacter;

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	NotJoined,
	None,
	Rusty,
	Vulcan,
	Zenith
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCharacterChanged, ECharacterType, Type);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerGetReady, bool, bReady);

USTRUCT()
struct FPlayerLobbyInformation
{
	GENERATED_BODY()

	UPROPERTY()
	bool bReady = false;

	UPROPERTY()
	ECharacterType ChoseCharacter = ECharacterType::NotJoined;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerCharacterChanged PlayerCharacterChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerGetReady PlayerGetReady;

	UPROPERTY()
	TObjectPtr<AOmochaLobbyCharacter> LobbyCharacter;
};

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaLobbyGameMode : public AOmochaBaseGameMode
{
	GENERATED_BODY()

public:
	AOmochaLobbyGameMode();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void ChoosePlayerCharacter(const FString& PlayerId, const ECharacterType Character);

	void GetReady(const FString& PlayerId, const bool Ready);

	void InitializePlayerInfo(AOmochaPlayerController* Player);

	UPROPERTY()
	TMap<FString, FPlayerLobbyInformation> Players;

	UPROPERTY(EditAnywhere)
	float CinematicStartDelay = 5.f;

protected:
	int32 PlayerStartIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	TArray<FString> PlayerStartTags;

	AOmochaLobbyCharacter* SpawnLobbyCharacter(const int32 Index, AController* OwnerController);

	UPROPERTY(EditDefaultsOnly)
	TArray<FTransform> SpawnTransforms;

	bool CheckEveryPlayerReadied();

	UPROPERTY(EditDefaultsOnly)
	TMap<ECharacterType, TSubclassOf<AOmochaPlayerCharacter>> CharacterClassType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AOmochaLobbyCharacter> LobbyCharacterClass;

	void SavePlayerCharacterInfo();

	FTimerHandle CinematicStartTimerHandle;

	void StartCinematicForAllPlayers();
};
