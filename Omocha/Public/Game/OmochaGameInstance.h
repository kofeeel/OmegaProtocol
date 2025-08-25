// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DataAsset/OmochaAbilityInfoDataAsset.h"
#include "Engine/GameInstance.h"
#include "OmochaGameInstance.generated.h"

class AWeaponPickupActor;
enum class EPlayerRestartState : uint8;
enum class ECharacterType : uint8;
class UOmochaEnemyDataAsset;
class AOmochaPlayerCharacter;
class AOmochaPlayerController;
class UGameplayAbility;
class UOmochaSoundDataAsset;

USTRUCT()
struct FSavedAttributes
{
	GENERATED_BODY()

	float Health = 0.f;

	float Resilience = 0.f;

	float AttackDamage = 0.f;

	float SkillDamage = 0.f;

	float Shield = 0.f;

	float MoveSpeed = 0.f;

	float AttackSpeed = 0.f;

	float CoolDownReduction = 0.f;

	float AttackRange = 0.f;

	float Level = 0.f;

	float CriticalChance = 0.f;

	float CriticalDamage = 0.f;

	float DodgeChance = 0.f;

	float ChargeLevel = 0.f;

	float MaxOmega = 0.f;

	float MaxHealth = 0.f;

	float MaxMoveSpeed = 0.f;

	float MaxAttackSpeed = 0.f;

	float MaxCoolDownReduction = 0.f;

	float MaxLevel = 0.f;

	float KnockbackResistance = 0.f;

	float LifeSteal = 0.f;

	// Zenith
	float ChainCount = 1;
	float ChargingTimeMultiplier = 2.f;
	float CanRotateWhileCharging = 0.f;

	// Vulcan
	float HasBuffRadius = 0.f;
	float BuffRadiusMultiplier = 1.f;
	float UltimateBezierCount = 5;

	// Rusty
	float HookSizeMultiplier = 1.f;
	float HookSpeedMultiplier = 1.f;
	float UltimateGravityPull = 0.f;
	float ShieldMaxHealth = 0.f;

	// Common
	float AttackPenetrationCount = 0;
	float MovementSkillRangeMultiplier = 1.f;
	float AttackProjectileSpeedMultiplier = 1.f;
};


USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY()
	FGameplayTag AbilityTag = FGameplayTag();
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

USTRUCT(BlueprintType)
struct FPlayerInformation
{
	GENERATED_BODY()

	UPROPERTY()
	bool bPlayed = false;

	UPROPERTY()
	bool bAttributesSaved = false;

	UPROPERTY()
	bool bAbilitiesSaved = false;

	UPROPERTY()
	FString DefaultStartTag = FString("Unknown");

	UPROPERTY()
	FString PlayerId = FString();

	UPROPERTY()
	uint64 SteamId = 0;

	UPROPERTY()
	FString PlayerName = FString("Unknown");

	UPROPERTY()
	TObjectPtr<AOmochaPlayerController> PlayerController;

	UPROPERTY()
	TSubclassOf<AOmochaPlayerCharacter> PlayerCharacterClass;

	UPROPERTY()
	FGameplayTag State;

	UPROPERTY()
	FSavedAttributes SavedAttributes;

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	UPROPERTY()
	TSubclassOf<AWeaponPickupActor> EquippedWeaponPickupClass;

	UPROPERTY()
	bool Winner = false;
};

USTRUCT(BlueprintType)
struct FGameScoreBoardData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool Winner = false;
};

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void SetOmega(const float InOmega) { Omega = InOmega; }
	float GetOmega() const { return Omega; }

	UPROPERTY(BlueprintReadWrite, Category = "Player Information")
	FPlayerInformation Player1;

	UPROPERTY(BlueprintReadWrite, Category = "Player Information")
	FPlayerInformation Player2;

	UPROPERTY(BlueprintReadWrite, Category = "Player Information")
	FPlayerInformation Player3;

	UPROPERTY(EditdefaultsOnly, Category = "All Abilities")
	TObjectPtr<UOmochaAbilityInfoDataAsset> AbilityInfo;

	int32 GetPlayerNumber(const FString& PlayerId) const;
	FGameplayTag GetPlayerState(const FString& PlayerId);
	FString GetPlayerStartTag(const FString& PlayerId);
	TSubclassOf<AOmochaPlayerCharacter> GetPlayerCharacterClass(const FString& PlayerId);
	TArray<FSavedAbility> GetPlayerAbilities(const FString& PlayerId) const;
	FSavedAttributes GetPlayerAttributes(const FString& PlayerId) const;
	FGameScoreBoardData GetPlayerScoreData(const FString& PlayerId) const;
	TSubclassOf<AWeaponPickupActor> GetEquippedWeapon(const FString& PlayerId) const;

	//TODO : throw it away 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound System")
	TObjectPtr<UOmochaSoundDataAsset> MainSoundDataAsset;

	//GameAnalystic
	void InitGameAnalystic();

	void InitializeAllOfPlayerInfos();

	bool IsPlayerAbilitySystemSaved(const FString& PlayerId) const;
	bool NewPlayer(const FPlayerInformation& PlayerInformation);
	void LogoutPlayer(const FString& PlayerId);
	bool CheckPlayerExist(const FString& PlayerId) const;
	bool ReconnectPlayer(const FPlayerInformation& PlayerInformation);

	void AllPlayerBroadcastLobbyLogin(const FString& PlayerId);
	void AllPlayerBroadcastLobbyCharacter(const FString& PlayerId, const ECharacterType Character);
	void AllPlayerBroadcastLobbyReady(const FString& PlayerId, const bool Ready);
	void AllPlayerBroadcastPlayGameOver();
	void AllPlayerBroadcastScoreState(const FString& PlayerId, const EPlayerRestartState& State);
	void AllPlayerBroadcastChatMessage(const FString& Message);

	void SavePlayerState(const FString& PlayerId, const FGameplayTag& PlayerState);
	void SavePlayerCharacterClass(const FString& PlayerId, const TSubclassOf<AOmochaPlayerCharacter>& CharacterClass);
	void SavePlayerAttributes(const FString& PlayerId, const FSavedAttributes& SavedAttributes);
	void EmptyPlayerAbilities(const FString& PlayerId);
	void SavePlayerAbility(const FString& PlayerId, const FSavedAbility& SavedAbility);
	void SavePlayerWeapon(const FString& PlayerId, TSubclassOf<AWeaponPickupActor> WeaponPickupClass);

	void ForAlarmPrepareForNextLevel(const bool bReady) const;

	static FString GetUniqueId(const APlayerState* PlayerState);

	UOmochaEnemyDataAsset* GetEnemyDA() const { return EnemyDataAsset; }

	void SetWinner(const FString& PlayerId);
	void SetLooser(const FString& PlayerId);
	void NeverWin();

	// Level System
	UPROPERTY(BlueprintReadOnly, Category = "Maintain Level")
	int32 MainTainTeamLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Maintain Level")
	int32 MainTainTeamXP = 0;

	UFUNCTION(BlueprintCallable, Category = "Level System")
	void UpdateMainTainData(int32 NewLevel, int32 NewXP);
	
	UFUNCTION(BlueprintPure, Category = "Level System")
	void GetMainTainLevelData(int32& OutLevel, int32& OutXP) const;

	// TEST
	UFUNCTION(Exec)
	void ShowMainTainLevel();

	UFUNCTION(Exec)
	void ResetMainTainLevel();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy System")
	TObjectPtr<UOmochaEnemyDataAsset> EnemyDataAsset;

private:
	void SetupDisplaySettings();
	float Omega = 0.f;

	void LoadLevelData();
	void SaveLevelData();
};
