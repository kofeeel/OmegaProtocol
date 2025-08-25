// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OmochaEnemyDataAsset.generated.h"

class UBehaviorTree;
class UGameplayAbility;
class UGameplayEffect;


UENUM(BlueprintType)
enum class EEnemyClass : uint8
{
	Infantry, 
	Sniper, 
	Tanker 
};

UENUM(BlueprintType)
enum class EEnemyRank : uint8
{
	Minion,
	Named,
	Mid, 
	Boss
};

USTRUCT(BlueprintType)
struct FEnemyAbility
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
};


USTRUCT(BlueprintType)
struct FEnemyClassInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FEnemyAbility> EnemyType;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};

USTRUCT(BlueprintType)
struct FEnemyRankData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TMap<EEnemyClass, FEnemyClassInfo> ClassData;
	
};
/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaEnemyDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Classes")
	TMap<EEnemyRank, FEnemyRankData> EnemyData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Common Abilities")
	TArray<TSubclassOf<UGameplayAbility> > CommonAbilities;
	
	UFUNCTION(BlueprintCallable, Category = "Enemy Data")
	FEnemyClassInfo GetEnemyClassInfo(EEnemyRank InRank, EEnemyClass InClass) const;


	UFUNCTION(BlueprintCallable, Category = "Enemy Data")
	TArray<TSubclassOf<UGameplayAbility>> GetEnemyAbilities(EEnemyRank InRank, EEnemyClass InClass, int32 EnemyIndex) const;
};
