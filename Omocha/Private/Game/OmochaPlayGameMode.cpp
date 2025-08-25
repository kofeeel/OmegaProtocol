// Copyright It's My Turn


#include "Game/OmochaPlayGameMode.h"

#include "Character/OmochaPlayerCharacter.h"
#include "Game/OmochaGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OmochaPlayerStart.h"
#include "Online/CoreOnline.h"
#include "Player/OmochaPlayerController.h"


AOmochaPlayGameMode::AOmochaPlayGameMode()
{
	bUseSeamlessTravel = true;
}

AActor* AOmochaPlayGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = Player->GetPlayerState<APlayerState>())
		{
			const FString StartTag = OmochaGameInstance->GetPlayerStartTag(UOmochaGameInstance::GetUniqueId(PlayerState));
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmochaPlayerStart::StaticClass(), Actors);
			if (Actors.Num() > 0)
			{
				AActor* SelectedActor = Actors[0];
				for (AActor* Actor : Actors)
				{
					if (AOmochaPlayerStart* PlayerStart = Cast<AOmochaPlayerStart>(Actor))
					{
						if (PlayerStart->PlayerStartTag == StartTag
							&& (PlayerStart->bCanBeOverlap || !PlayerStart->bIsSpawned))
						{
							PlayerStart->bIsSpawned = true;
							SelectedActor = PlayerStart;
							break;
						}
					}
				}
				return SelectedActor;
			}
		}
	}
	return nullptr;
}

void AOmochaPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		NewPlayer->SetInputMode(FInputModeGameOnly());
	}
}

void AOmochaPlayGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
		{
			if (OmochaGameInstance->CheckPlayerExist(UOmochaGameInstance::GetUniqueId(PlayerState)))
			{
				FPlayerInformation PlayerInformation;
				PlayerInformation.PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
				PlayerInformation.PlayerName = PlayerState->GetPlayerName();
				PlayerInformation.PlayerController = Cast<AOmochaPlayerController>(NewPlayer);
				if (OmochaGameInstance->ReconnectPlayer(PlayerInformation))
				{
					PlayersStatus.Add(PlayerInformation.PlayerId, true);
					return;
				}
			}
		}
		KickPlayerWithReason(NewPlayer, FString("Player does not exist."));
	}
}

void AOmochaPlayGameMode::Logout(AController* Exiting)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
		{
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
			PlayersStatus.Remove(PlayerId);
			OmochaGameInstance->LogoutPlayer(PlayerId);
		}
	}
	
	Super::Logout(Exiting);
}

APawn* AOmochaPlayGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
		{
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PlayerState);
			if (GetWorld() && StartSpot)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				SpawnParams.Owner = NewPlayer;
				SpawnParams.Instigator = NewPlayer->GetPawn();
				return GetWorld()->SpawnActor<APawn>(
					OmochaGameInstance->GetPlayerCharacterClass(PlayerId),
					StartSpot->GetTransform(),
					SpawnParams);
			}
		}
	}
	return nullptr;
}

void AOmochaPlayGameMode::TravelToScoreBoard()
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		OmochaGameInstance->AllPlayerBroadcastPlayGameOver();
		SetNextMap(ScoreBoardMap);
		TravelToMap();
	}
}

void AOmochaPlayGameMode::PlayerDied(APlayerState* PS)
{
	if (PS)
	{
		const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
		if (PlayersStatus.Contains(PlayerId) && PlayersStatus[PlayerId])
		{
			PlayersStatus[PlayerId] = false;
			if (CheckGameOver())
			{
				FTimerManager& TimerManager = GetWorld()->GetTimerManager();
				TimerManager.ClearTimer(CheckTimerHandle);
				TimerManager.SetTimer(CheckTimerHandle, this, &ThisClass::TravelToScoreBoard, CheckTimeAfterDied, false);
			}
		}
	}
}

void AOmochaPlayGameMode::PlayerRebirth(APlayerState* PS)
{
	if (PS)
	{
		const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
		if (PlayersStatus.Contains(PlayerId) && !PlayersStatus[PlayerId])
		{
			PlayersStatus[PlayerId] = true;
			FTimerManager& TimerManager = GetWorld()->GetTimerManager();
			TimerManager.ClearTimer(CheckTimerHandle);
		}
	}
}

bool AOmochaPlayGameMode::CheckGameOver()
{
	for (auto& Player : PlayersStatus)
	{
		if (Player.Value == true) return false;
	}
	return true;
}
