// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OmochaGameModeBase.generated.h"

class UOmochaEnemyDataAsset;
class UOmochaCharacterDataAsset;
class AOmochaPlayerController;
class AOmochaPlayerCharacter;
/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOmochaGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UFUNCTION(BlueprintCallable, Category = "Character Assignment")
	TSubclassOf<AOmochaPlayerCharacter> AssignCharacterClassToPlayer(AOmochaPlayerController* PlayerController);


	UFUNCTION(BlueprintCallable, Category = "Omega System")
	bool TransferNewOmegaToController(AOmochaPlayerController* NewOmegaPlayerController);

	UFUNCTION(BlueprintCallable, Category = "Omega System")
	AOmochaPlayerController* GetCurrentOmegaPlayer() const { return CurrentOmegaPlayerController; }

	UFUNCTION(BlueprintCallable, Category = "Omega System")
	bool IsControllerrOmega(AOmochaPlayerController* PlayerController) const
	{
		return PlayerController == CurrentOmegaPlayerController;
	}

	AOmochaPlayerCharacter* GetCharacterFromController(AOmochaPlayerController* Controller);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy System")
	TObjectPtr<UOmochaEnemyDataAsset> EnemyDataAsset;

	UFUNCTION(BlueprintCallable, Category = "Enemy System")
	UOmochaEnemyDataAsset* GetEnemyDA() const { return EnemyDataAsset; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Classes")
	TArray<TSubclassOf<AOmochaPlayerCharacter>> AvailableCharacterClasses;

	UPROPERTY(BlueprintReadOnly, Category = "Character Classes")
	TMap<AOmochaPlayerController*, TSubclassOf<AOmochaPlayerCharacter>> PlayerCharacterClassMap;

	UPROPERTY(BlueprintReadOnly, CateGory = "Omega System")
	AOmochaPlayerController* CurrentOmegaPlayerController = nullptr;

	UPROPERTY(BlueprintReadOnly, CateGory = "Player Management")
	TArray<AOmochaPlayerController*> ConnectedPlayerControllers;

	UFUNCTION()
	void AssignRandomOmegaPlayer();

	UFUNCTION()
	void CheckGameStart();
};
