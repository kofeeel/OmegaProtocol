// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaBaseGameMode.h"
#include "OmochaPlayGameMode.generated.h"



class UOmochaEnemyDataAsset;
/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaPlayGameMode : public AOmochaBaseGameMode
{
	GENERATED_BODY()

public:
	AOmochaPlayGameMode();
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
	
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
	void TravelToScoreBoard();

	virtual void PlayerDied(APlayerState* PS);

	virtual void PlayerRebirth(APlayerState* PS);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CheckTimeAfterDied = 3.f;
	FTimerHandle CheckTimerHandle;
	
	TMap<FString, bool> PlayersStatus;

	virtual bool CheckGameOver();
};
