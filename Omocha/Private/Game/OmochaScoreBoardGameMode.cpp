// Copyright It's My Turn


#include "Game/OmochaScoreBoardGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Player/OmochaPlayerController.h"

AOmochaScoreBoardGameMode::AOmochaScoreBoardGameMode()
{
	bUseSeamlessTravel = true;
}

void AOmochaScoreBoardGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
		{
			FPlayerInformation PlayerInformation;
			PlayerInformation.PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
			PlayerInformation.PlayerName = PlayerState->GetPlayerName();
			PlayerInformation.PlayerController = Cast<AOmochaPlayerController>(NewPlayer);
			if (OmochaGameInstance->ReconnectPlayer(PlayerInformation))
			{
				OmochaGameInstance->AllPlayerBroadcastScoreState(PlayerInformation.PlayerId, EPlayerRestartState::NotReady);
				PlayerScores.Add(PlayerInformation.PlayerId, OmochaGameInstance->GetPlayerScoreData(PlayerInformation.PlayerId));
				PlayerStatuses.Add(PlayerInformation.PlayerId, EPlayerRestartState::NotReady);
				return;
			}
		}
		KickPlayerWithReason(NewPlayer, FString("A player is not valid."));
	}
}

void AOmochaScoreBoardGameMode::Logout(AController* Exiting)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
		{
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
			OmochaGameInstance->AllPlayerBroadcastScoreState(PlayerId, EPlayerRestartState::Leave);
			OmochaGameInstance->LogoutPlayer(PlayerId);
			CancelTravel();
			PlayerScores.Remove(PlayerId);
			PlayerStatuses.Remove(PlayerId);
		}
	}
	Super::Logout(Exiting);
}

FGameScoreBoardData AOmochaScoreBoardGameMode::GetScoreBoardData(APlayerState* PS)
{
	const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
	if (PlayerScores.Contains(PlayerId))
	{
		return PlayerScores[PlayerId];
	}
	return FGameScoreBoardData();
}

void AOmochaScoreBoardGameMode::AllOfPlayerStatesRequest(AOmochaPlayerController* PlayerController)
{
	if (PlayerController)
	{
		if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
		{
			for (auto& Player: PlayerStatuses)
			{
				const int32 PlayerNumber = OmochaGameInstance->GetPlayerNumber(Player.Key);
				PlayerController->Client_PlayerRestartState(PlayerNumber, Player.Value);
			}
		}
	}
}

void AOmochaScoreBoardGameMode::PlayerRestartReady(APlayerState* PS, bool bReady)
{
	if (!PS) return;
	
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
		if (PlayerStatuses.Contains(PlayerId))
		{
			if (bReady)
			{
				OmochaGameInstance->AllPlayerBroadcastScoreState(PlayerId, EPlayerRestartState::Ready);
				PlayerStatuses[PlayerId] = EPlayerRestartState::Ready;
				for (auto& Player: PlayerStatuses)
				{
					if (Player.Value != EPlayerRestartState::Ready) return;
				}
				TravelToMap(false, true);
			}
			else
			{
				OmochaGameInstance->AllPlayerBroadcastScoreState(PlayerId, EPlayerRestartState::NotReady);
				PlayerStatuses[PlayerId] = EPlayerRestartState::NotReady;
				CancelTravel();
			}	
		}
	}
}
