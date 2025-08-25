// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/OmochaEnemyDataAsset.h"
#include "Engine/TargetPoint.h"
#include "EnemySpawnPoint.generated.h"

class AOmochaEnemy;
class UNiagaraSystem;

UCLASS()
class OMOCHA_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawnPoint();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void StartSpawning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Repetition")
	bool bRepeatSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Repetition",
		meta = (EditCondition = "bRepeatSpawn", ClampMin = "0"))
	int32 NumberOfSpawns = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Repetition",
		meta = (EditCondition = "bRepeatSpawn", ClampMin = "0.1"))
	float SpawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config")
	TSubclassOf<AOmochaEnemy> EnemyActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config")
	EEnemyRank EnemyRank = EEnemyRank::Minion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config")
	EEnemyClass EnemyClass = EEnemyClass::Infantry;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config")
	uint8 EnemyIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning|Effect")
	UNiagaraSystem* PreSpawnEffect;

protected:
	UPROPERTY(EditAnywhere, Category = "Spawner Settings | Data Tables")
	UDataTable* LevelDropRateTable;

	UPROPERTY(EditAnywhere, Category = "Spawner Settings | Data Tables")
	UDataTable* RareItemTable;

	UPROPERTY(EditAnywhere, Category = "Spawner Settings | Data Tables")
	UDataTable* EpicItemTable;

	UPROPERTY(EditAnywhere, Category = "Spawner Settings | Data Tables")
	UDataTable* LegendaryItemTable;

private:
	void PrepareToSpawn();
	void SpawnEnemy();

	FTimerHandle SpawnTimerHandle;

	int32 CurrentSpawnCount = 0;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayPreSpawnEffect();
};
