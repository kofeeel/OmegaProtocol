// Copyright It's My Turn
#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OmochaSoundDataAsset.generated.h"

// Situational sound configuration
USTRUCT(BlueprintType)
struct OMOCHA_API FSituationalSound
{
	GENERATED_BODY()

	// Situation identifier (e.g., "Situation.Combat", "Situation.Boss")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	FGameplayTag SituationTag;

	// Sound asset to play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	USoundBase* SoundAsset = nullptr;

	// Volume multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Volume = 1.0f;

	// Fade in duration when starting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config", meta = (ClampMin = "0.0"))
	float FadeInTime = 2.0f;

	// Fade out duration when stopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config", meta = (ClampMin = "0.0"))
	float FadeOutTime = 2.0f;

	// Priority for overlapping situations (higher = more important)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Config")
	int32 Priority = 0;

	FSituationalSound()
	{
		SituationTag = FGameplayTag::EmptyTag;
		SoundAsset = nullptr;
		Volume = 1.0f;
		FadeInTime = 2.0f;
		FadeOutTime = 2.0f;
		Priority = 0;
	}
};

// Level-specific sound data
USTRUCT(BlueprintType)
struct OMOCHA_API FLevelSoundData
{
	GENERATED_BODY()

	// Default BGM for this level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Sound")
	USoundBase* DefaultBGM = nullptr;

	// Default volume for level BGM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Sound", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DefaultVolume = 1.0f;

	// Situational sounds specific to this level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Sound")
	TArray<FSituationalSound> SituationalSounds;

	// Whether to use global situational sounds in addition to level-specific ones
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Sound")
	bool bUseGlobalSituationalSounds = true;

	FLevelSoundData()
	{
		DefaultBGM = nullptr;
		DefaultVolume = 1.0f;
		bUseGlobalSituationalSounds = true;
	}
};

// Level to sound mapping
USTRUCT(BlueprintType)
struct OMOCHA_API FLevelSoundMapping
{
	GENERATED_BODY()

	// Level identifier (e.g., "Level.Village", "Level.Dungeon")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Mapping")
	FGameplayTag LevelTag;

	// Sound data for this level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Mapping")
	FLevelSoundData SoundData;

	// Display name for editor (debugging purposes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Mapping")
	FString DisplayName;

	FLevelSoundMapping()
	{
		LevelTag = FGameplayTag::EmptyTag;
		DisplayName = TEXT("Unnamed Level");
	}
};

/**
 * Data Asset containing all level and situational sound configurations
 */
UCLASS(BlueprintType)
class OMOCHA_API UOmochaSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Constructor
	UOmochaSoundDataAsset();

	// Level sound mappings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	TArray<FLevelSoundMapping> LevelSoundMappings;

	// Global situational sounds (available in all levels)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	TArray<FSituationalSound> GlobalSituationalSounds;

	// Default fallback BGM when no level-specific BGM is found
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Data")
	USoundBase* FallbackBGM = nullptr;
	// Find level sound data by level tag
	FLevelSoundData* FindLevelSoundData(FGameplayTag LevelTag);

	// Find situational sound for specific level and situation
	FSituationalSound* FindSituationalSound(FGameplayTag LevelTag, FGameplayTag SituationTag);

	// Get all available level tags
	UFUNCTION(BlueprintCallable, Category = "Sound Data")
	TArray<FGameplayTag> GetAllLevelTags() const;

	// Get all situation tags for a specific level
	UFUNCTION(BlueprintCallable, Category = "Sound Data")
	TArray<FGameplayTag> GetSituationTagsForLevel(FGameplayTag LevelTag) const;

	// Validate data asset setup
	UFUNCTION(BlueprintCallable, Category = "Sound Data")
	bool ValidateSetup(TArray<FString>& OutErrorMessages) const;
};
