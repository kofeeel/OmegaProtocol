// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Game/OmochaBaseGameMode.h"
#include "OmochaScoreBoardGameMode.generated.h"


UENUM(BlueprintType)
enum class EPlayerRestartState : uint8
{
	None,
	Leave,
	NotReady,
	Ready
};

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaScoreBoardGameMode : public AOmochaBaseGameMode
{
	GENERATED_BODY()

public:
	AOmochaScoreBoardGameMode();
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	FGameScoreBoardData GetScoreBoardData(APlayerState* PS);
	void AllOfPlayerStatesRequest(AOmochaPlayerController* PlayerController);
	void PlayerRestartReady(APlayerState* PS, bool bReady);

protected:
	TMap<FString, FGameScoreBoardData> PlayerScores;
	TMap<FString, EPlayerRestartState> PlayerStatuses;
};
