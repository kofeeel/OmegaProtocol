// Copyright It's My Turn


#include "Game/OmochaLobbyGameMode.h"
#include "Game/OmochaGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OmochaPlayerController.h"
#include "Player/OmochaPlayerStart.h"
#include "Online/CoreOnline.h"
#include "Character/OmochaLobbyCharacter.h"
#include "Character/OmochaPlayerCharacter.h"

AOmochaLobbyGameMode::AOmochaLobbyGameMode()
{
	bUseSeamlessTravel = true;
}

AActor* AOmochaLobbyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (PlayerStartTags.Num() == 0) {
		return nullptr;
	}

	PlayerStartIndex %= PlayerStartTags.Num();
	const FString StartTag = PlayerStartTags[PlayerStartIndex];
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmochaPlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0) {
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors) {
			if (AOmochaPlayerStart* PlayerStart = Cast<AOmochaPlayerStart>(Actor)) {
				if (PlayerStart->PlayerStartTag == StartTag
					&& (PlayerStart->bCanBeOverlap || !PlayerStart->bIsSpawned)) {
					PlayerStart->bIsSpawned = true;
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		PlayerStartIndex++;
		return SelectedActor;
	}
	return nullptr;
}

void AOmochaLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AOmochaLobbyGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (NewPlayer)
	{
		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		NewPlayer->SetInputMode(InputModeData);
		NewPlayer->SetShowMouseCursor(true);
	}
	
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
		{
			FPlayerInformation PlayerInformation;
			PlayerInformation.PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
			PlayerInformation.SteamId =  FCString::Atoi64(*PlayerInformation.PlayerId);
			PlayerInformation.PlayerName = PlayerState->GetPlayerName();
			PlayerInformation.PlayerController = Cast<AOmochaPlayerController>(NewPlayer);
			if (OmochaGameInstance->NewPlayer(PlayerInformation))
			{
				FPlayerLobbyInformation& PlayerLobbyInformation = Players.Add(
					PlayerInformation.PlayerId, FPlayerLobbyInformation());
				PlayerLobbyInformation.ChoseCharacter = ECharacterType::None;

				PlayerLobbyInformation.LobbyCharacter = SpawnLobbyCharacter(
					OmochaGameInstance->GetPlayerNumber(PlayerInformation.PlayerId), NewPlayer);

				if (PlayerLobbyInformation.LobbyCharacter)
				{
					PlayerLobbyInformation.PlayerCharacterChanged.AddDynamic(
						PlayerLobbyInformation.LobbyCharacter, &AOmochaLobbyCharacter::ChooseCharacter);
					PlayerLobbyInformation.PlayerGetReady.AddDynamic(PlayerLobbyInformation.LobbyCharacter,
																	 &AOmochaLobbyCharacter::GetReady);
					for (auto& Player : Players)
					{
						GetReady(Player.Key, false);
					}
					OmochaGameInstance->AllPlayerBroadcastLobbyLogin(PlayerInformation.PlayerId);
					return;
				}
			}
		}
		KickPlayerWithReason(NewPlayer, FString("A new player is not valid."));
	}
}

void AOmochaLobbyGameMode::Logout(AController* Exiting)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>()) {
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
			ChoosePlayerCharacter(PlayerId, ECharacterType::NotJoined);
			OmochaGameInstance->LogoutPlayer(PlayerId);
		}
	}
	Super::Logout(Exiting);
}

AOmochaLobbyCharacter* AOmochaLobbyGameMode::SpawnLobbyCharacter(const int32 Index, AController* OwnerController)
{
	if (UWorld* World = GetWorld()) {
		if (SpawnTransforms.Num() > Index) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerController;

			return World->SpawnActor<AOmochaLobbyCharacter>(LobbyCharacterClass, SpawnTransforms[Index], SpawnParams);
		}
	}
	return nullptr;
}

bool AOmochaLobbyGameMode::CheckEveryPlayerReadied()
{
	for (auto& Player : Players) {
		if (Player.Value.bReady == false) {
			return false;
		}
	}
	return true;
}

void AOmochaLobbyGameMode::SavePlayerCharacterInfo()
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		for (auto& Player : Players) {
			if (OmochaGameInstance->CheckPlayerExist(Player.Key)
				&& CharacterClassType.Contains(Player.Value.ChoseCharacter)) {
				OmochaGameInstance->SavePlayerCharacterClass(
					Player.Key, CharacterClassType[Player.Value.ChoseCharacter]);
			}
		}
	}
}

void AOmochaLobbyGameMode::StartCinematicForAllPlayers()
{
	for (auto& PlayerInfoPair : Players)
	{
		if (PlayerInfoPair.Value.LobbyCharacter)
		{
			PlayerInfoPair.Value.LobbyCharacter->Multicast_StartRunCinematic();
		}
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (AOmochaPlayerCharacter* PossessedPawn = Cast<AOmochaPlayerCharacter>(PC->GetPawn()))
			{
				PossessedPawn->Client_StartLobbyCameraCinematic();
			}
		}
	}

	SavePlayerCharacterInfo();
	GetWorld()->GetTimerManager().SetTimer(
				CinematicStartTimerHandle,
				this,
				&AOmochaLobbyGameMode::TravelNextLevel,
				CinematicStartDelay,
				false);
}

void AOmochaLobbyGameMode::GetReady(const FString& PlayerId, const bool Ready)
{
	if (Players.Contains(PlayerId)) {
		Players[PlayerId].bReady = Ready;
		Players[PlayerId].PlayerGetReady.Broadcast(Ready);
		if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
		{
			OmochaGameInstance->AllPlayerBroadcastLobbyReady(PlayerId, Ready);
		}
		if (Ready) {
			if (CheckEveryPlayerReadied()) {
				GetPrepareForNextLevel(true);

				GetWorld()->GetTimerManager().SetTimer(
					CinematicStartTimerHandle,
					this,
					&AOmochaLobbyGameMode::StartCinematicForAllPlayers,
					CinematicStartDelay,
					false);
			}
		}
		else {
			GetPrepareForNextLevel(false);
			GetWorld()->GetTimerManager().ClearTimer(CinematicStartTimerHandle);
		}
	}
}

void AOmochaLobbyGameMode::InitializePlayerInfo(AOmochaPlayerController* Player)
{
	if (Player)
	{
		if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
		{
			if (OmochaGameInstance->Player1.bPlayed)
			{
				Player->Client_InitializeResponse(
					0,
					OmochaGameInstance->Player1.PlayerName,
					OmochaGameInstance->Player1.PlayerId,
					Players[OmochaGameInstance->Player1.PlayerId].ChoseCharacter,
					Players[OmochaGameInstance->Player1.PlayerId].bReady);
			}
			if (OmochaGameInstance->Player2.bPlayed)
			{
				Player->Client_InitializeResponse(
					1,
					OmochaGameInstance->Player2.PlayerName,
					OmochaGameInstance->Player2.PlayerId,
					Players[OmochaGameInstance->Player2.PlayerId].ChoseCharacter,
					Players[OmochaGameInstance->Player2.PlayerId].bReady);
			}
			if (OmochaGameInstance->Player3.bPlayed)
			{
				Player->Client_InitializeResponse(
					2,
					OmochaGameInstance->Player3.PlayerName,
					OmochaGameInstance->Player3.PlayerId,
					Players[OmochaGameInstance->Player3.PlayerId].ChoseCharacter,
					Players[OmochaGameInstance->Player3.PlayerId].bReady);
			}
		}
		for (auto& Player : Players)
		{
			GetReady(Player.Key, Player.Value.bReady);
			const ECharacterType CharacterType = Player.Value.ChoseCharacter;
			ChoosePlayerCharacter(Player.Key, ECharacterType::None);
			ChoosePlayerCharacter(Player.Key, CharacterType);
		}
	}
}

void AOmochaLobbyGameMode::ChoosePlayerCharacter(const FString& PlayerId, const ECharacterType Character)
{
	if (Players.Contains(PlayerId))
	{
		Players[PlayerId].ChoseCharacter = Character;
		Players[PlayerId].LobbyCharacter->MyCharacterType = Character;
		Players[PlayerId].PlayerCharacterChanged.Broadcast(Players[PlayerId].ChoseCharacter);
		if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
		{
			OmochaGameInstance->AllPlayerBroadcastLobbyCharacter(PlayerId, Character);
		}
		if (Character == ECharacterType::NotJoined)
		{
			Players[PlayerId].LobbyCharacter->Destroy();
			Players.Remove(PlayerId);
		}
	}
}
