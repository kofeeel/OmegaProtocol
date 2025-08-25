// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Game/OmochaPlayGameMode.h"
#include "OmochaFinalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaFinalGameMode : public AOmochaPlayGameMode
{
	GENERATED_BODY()

public:
	AOmochaFinalGameMode();
	
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	virtual void PlayerDied(APlayerState* PS) override;

	virtual void PlayerRebirth(APlayerState* PS) override;

	UFUNCTION(BlueprintCallable)
	void SetWinner(APlayerState* PS);

	UFUNCTION(BlueprintCallable)
	void SetLooser(APlayerState* PS);
	

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bHighlander = false;

	bool bTravel = false;
};
