// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/OmochaGameModeBase.h"

#include "OmochaGameplayTags.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaCharacterConfigComponent.h"
#include "Player/OmochaPlayerController.h"
#include "Player/OmochaPlayerState.h"

AOmochaGameModeBase::AOmochaGameModeBase()
{
	PlayerControllerClass = AOmochaPlayerController::StaticClass();
	PlayerStateClass = AOmochaPlayerState::StaticClass();
}

void AOmochaGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (GameState)
	{
		if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(NewPlayer))
		{
			ConnectedPlayerControllers.AddUnique(PlayerController);
			CheckGameStart();
		}
	}
}

void AOmochaGameModeBase::Logout(AController* Exiting)
{
	if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(Exiting))
	{
		ConnectedPlayerControllers.Remove(PlayerController);

		if (PlayerController == CurrentOmegaPlayerController)
		{
			CurrentOmegaPlayerController = nullptr;

			if (ConnectedPlayerControllers.Num() > 0)
			{
				AssignRandomOmegaPlayer();
			}
		}
	}

	Super::Logout(Exiting);
}

TSubclassOf<AOmochaPlayerCharacter> AOmochaGameModeBase::AssignCharacterClassToPlayer(
	AOmochaPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return nullptr;
	}

	if (PlayerCharacterClassMap.Contains(PlayerController))
	{
		return PlayerCharacterClassMap[PlayerController];
	}

	TArray<TSubclassOf<AOmochaPlayerCharacter>> UsedClasses;
	for (auto& Pair : PlayerCharacterClassMap)
	{
		UsedClasses.AddUnique(Pair.Value);
	}

	for (TSubclassOf<AOmochaPlayerCharacter> CharacterClass : AvailableCharacterClasses)
	{
		if (CharacterClass && !UsedClasses.Contains(CharacterClass))
		{
			PlayerCharacterClassMap.Add(PlayerController, CharacterClass);
			return CharacterClass;
		}
	}

	if (AvailableCharacterClasses.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableCharacterClasses.Num() - 1);
		TSubclassOf<AOmochaPlayerCharacter> RandomClass = AvailableCharacterClasses[RandomIndex];
		PlayerCharacterClassMap.Add(PlayerController, RandomClass);
		return RandomClass;
	}

	return nullptr;
}

APawn* AOmochaGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	// Plater Character Spawn Logic
	
	if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(NewPlayer))
	{
		TSubclassOf<AOmochaPlayerCharacter> AssignedClass = AssignCharacterClassToPlayer(PlayerController);

		if (AssignedClass)
		{
			// Start Spot (0,0,0)
			FVector SpawnLocation = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
			FRotator SpawnRotation = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;

			// Actor Create Option
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			APawn* SpawnedPawn = GetWorld()->SpawnActor<
				APawn>(AssignedClass, SpawnLocation, SpawnRotation, SpawnParams);

			return SpawnedPawn;
		}
	}

	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

void AOmochaGameModeBase::CheckGameStart()
{
	if (ConnectedPlayerControllers.Num() >= 3)
	{
		TArray<AOmochaPlayerCharacter*> ReadyCharacters;
		for (AOmochaPlayerController* Controller : ConnectedPlayerControllers)
		{
			if (AOmochaPlayerCharacter* Character = GetCharacterFromController(Controller))
			{
				ReadyCharacters.Add(Character);
			}
		}

		if (ReadyCharacters.Num() >= 3)
		{
			AssignRandomOmegaPlayer();
		}
		else
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this,
			                                       &AOmochaGameModeBase::CheckGameStart, 1.0f, false);
		}
	}
}

void AOmochaGameModeBase::AssignRandomOmegaPlayer()
{
	if (ConnectedPlayerControllers.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No connected players to assign Omega!"));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, ConnectedPlayerControllers.Num() - 1);
	AOmochaPlayerController* NewOmegaPlayerController = ConnectedPlayerControllers[RandomIndex];

	// Transfer the new Omega to the selected controller
	TransferNewOmegaToController(NewOmegaPlayerController);
}

bool AOmochaGameModeBase::TransferNewOmegaToController(AOmochaPlayerController* NewOmegaPlayerController)
{
	if (!NewOmegaPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("TransferNewOmegaToController: NewOmegaPlayerController is nullptr"));
		return false;
	}

	if (!ConnectedPlayerControllers.Contains(NewOmegaPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("TransferNewOmegaToController: NewOmegaPlayerController is not connected!"));
		return false;
	}

	if (CurrentOmegaPlayerController && CurrentOmegaPlayerController != NewOmegaPlayerController)
	{
		if (AOmochaPlayerCharacter* OldCharacter = GetCharacterFromController(CurrentOmegaPlayerController))
		{
			if (UOmochaCharacterConfigComponent* OldConfigComp =
				OldCharacter->FindComponentByClass<UOmochaCharacterConfigComponent>())
			{
				OldConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Normal);
			}
		}
	}

	CurrentOmegaPlayerController = NewOmegaPlayerController;

	if (AOmochaPlayerCharacter* NewCharacter = GetCharacterFromController(CurrentOmegaPlayerController))
	{
		if (UOmochaCharacterConfigComponent* NewConfigComp =
			NewCharacter->FindComponentByClass<UOmochaCharacterConfigComponent>())
		{
			NewConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Omega);
		}
	}
	else
	{
		return false;
	}
	return true;
}

AOmochaPlayerCharacter* AOmochaGameModeBase::GetCharacterFromController(AOmochaPlayerController* Controller)
{
	if (!Controller)
	{
		return nullptr;
	}

	if (APawn* Pawn = Controller->GetPawn())
	{
		return Cast<AOmochaPlayerCharacter>(Pawn);
	}

	return nullptr;
}
