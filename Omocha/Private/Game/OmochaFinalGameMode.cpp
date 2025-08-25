// Copyright It's My Turn


#include "Game/OmochaFinalGameMode.h"

#include "Character/OmochaPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Omocha/Omocha.h"

AOmochaFinalGameMode::AOmochaFinalGameMode()
{
	bUseSeamlessTravel = true;
}

APawn* AOmochaFinalGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn* PlayerPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

	if (AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(PlayerPawn))
	{
		const FName& PlayerTag = FName(TEXT("Player"));
		if (PlayerCharacter->ActorHasTag(PlayerTag))
		{
			PlayerCharacter->Tags.Remove(PlayerTag);
		}
		PlayerCharacter->Tags.Add("Fight");
		PlayerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ChainTrace, ECR_Overlap);
	}

	return PlayerPawn;
}

void AOmochaFinalGameMode::PlayerDied(APlayerState* PS)
{
	Super::PlayerDied(PS);
	
	if (bHighlander && PS)
	{
		int32 DeathCount = 0;
		FString Winner = FString();
		for (auto& Player: PlayersStatus)
		{
			if (Player.Value == false)
			{
				++DeathCount;
			}
			else
			{
				Winner = Player.Key;
			}
		}
		if (PlayersStatus.Num() - DeathCount == 1 && bTravel == false)
		{
			bTravel = true;
			if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
			{
				OmochaGameInstance->SetWinner(Winner);
			}
			TravelToMap(true);
		}
	}
}

void AOmochaFinalGameMode::PlayerRebirth(APlayerState* PS)
{
	Super::PlayerRebirth(PS);

	if (PS && bHighlander)
	{
		const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
		if (PlayersStatus.Contains(PlayerId) && PlayersStatus[PlayerId] && bTravel)
		{
			CancelTravel();
			if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
			{
				OmochaGameInstance->NeverWin();
			}
			bTravel = false;
		}
	}
}

void AOmochaFinalGameMode::SetWinner(APlayerState* PS)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (PS)
		{
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
			OmochaGameInstance->SetWinner(PlayerId);
		}
	}
}

void AOmochaFinalGameMode::SetLooser(APlayerState* PS)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		if (PS)
		{
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
			OmochaGameInstance->SetLooser(PlayerId);
		}
	}
}
