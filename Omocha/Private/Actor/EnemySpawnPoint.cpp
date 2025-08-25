// Copyright It's My Turn


#include "Actor/EnemySpawnPoint.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/OmochaEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Actor/Item/DropManager.h"

AEnemySpawnPoint::AEnemySpawnPoint()
{
	bReplicates = true;
	SetReplicates(true);
	SetReplicateMovement(false);

	LevelDropRateTable = nullptr;
	RareItemTable = nullptr;
	EpicItemTable = nullptr;
	LegendaryItemTable = nullptr;
	static ConstructorHelpers::FObjectFinder<UDataTable> LevelDropRateTableFinder(
		TEXT("/Game/Blueprint/Data/Item/DT_LevelDropRates.DT_LevelDropRates"));
	if (LevelDropRateTableFinder.Succeeded())
	{
		LevelDropRateTable = LevelDropRateTableFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> RareItemTableFinder(
		TEXT("/Game/Blueprint/Data/Item/DT_RareItems.DT_RareItems"));
	if (RareItemTableFinder.Succeeded())
	{
		RareItemTable = RareItemTableFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> EpicItemTableFinder(
		TEXT("/Game/Blueprint/Data/Item/DT_EpicItems.DT_EpicItems"));
	if (EpicItemTableFinder.Succeeded())
	{
		EpicItemTable = EpicItemTableFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> LegendaryItemTableFinder(
		TEXT("/Game/Blueprint/Data/Item/DT_LegendaryItems.DT_LegendaryItems"));
	if (LegendaryItemTableFinder.Succeeded())
	{
		LegendaryItemTable = LegendaryItemTableFinder.Object;
	}
}

void AEnemySpawnPoint::StartSpawning()
{
	CurrentSpawnCount = 0;

	if (bRepeatSpawn)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AEnemySpawnPoint::PrepareToSpawn,
			SpawnDelay,
			true,
			0.0f
		);
	}
	else
	{
		PrepareToSpawn();
	}
}

void AEnemySpawnPoint::PrepareToSpawn()
{
	if (bRepeatSpawn)
	{
		if (NumberOfSpawns > 0 && CurrentSpawnCount >= NumberOfSpawns)
		{
			GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
			return;
		}
		CurrentSpawnCount++;
	}

	Multicast_PlayPreSpawnEffect();

	FTimerHandle TempTimer;
	GetWorld()->GetTimerManager().SetTimer(
		TempTimer,
		this,
		&AEnemySpawnPoint::SpawnEnemy,
		2.f,
		false
	);
}

void AEnemySpawnPoint::SpawnEnemy()
{
	if (!EnemyActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Set EnemyActorClass"));
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	FTransform SpawnTransform = GetActorTransform();

	AOmochaEnemy* SpawnedEnemy = GetWorld()->SpawnActorDeferred<AOmochaEnemy>(
		EnemyActorClass,
		GetActorTransform(),
		GetOwner(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (SpawnedEnemy)
	{
		const float CapsuleHalfHeight = SpawnedEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector AdjustedLocation = SpawnTransform.GetLocation() + FVector(0.f, 0.f, CapsuleHalfHeight);
		SpawnTransform.SetLocation(AdjustedLocation);

		SpawnedEnemy->EnemyRank = EnemyRank;
		SpawnedEnemy->EnemyClass = EnemyClass;
		SpawnedEnemy->EnemyIdx = EnemyIndex;
		SpawnedEnemy->SetPlayerLevel(Level);
		TSubclassOf<AOmochaEffectActor> DeterminedDropItemClass = UDropManager::GetDropItemClassByLevel(
			Level,
			LevelDropRateTable,
			RareItemTable,
			EpicItemTable,
			LegendaryItemTable
		);
		SpawnedEnemy->SetDropItemClass(DeterminedDropItemClass);
		SpawnedEnemy->FinishSpawning(SpawnTransform);

		SpawnedEnemy->SpawnDefaultController();
	}
}

void AEnemySpawnPoint::Multicast_PlayPreSpawnEffect_Implementation()
{
	if (PreSpawnEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PreSpawnEffect, GetActorLocation());
	}
}
