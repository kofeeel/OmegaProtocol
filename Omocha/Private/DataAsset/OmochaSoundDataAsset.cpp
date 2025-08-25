// Copyright It's My Turn

#include "DataAsset/OmochaSoundDataAsset.h"
#include "Engine/Engine.h"

UOmochaSoundDataAsset::UOmochaSoundDataAsset()
{
	FallbackBGM = nullptr;
}

FLevelSoundData* UOmochaSoundDataAsset::FindLevelSoundData(FGameplayTag LevelTag)
{
	if (!LevelTag.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("FindLevelSoundData: Invalid LevelTag"));
		return nullptr;
	}

	// Simple array search
	for (FLevelSoundMapping& Mapping : LevelSoundMappings) {
		if (Mapping.LevelTag.MatchesTagExact(LevelTag)) {
			return &Mapping.SoundData;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("FindLevelSoundData: No data found for level tag: %s"), *LevelTag.ToString());
	return nullptr;
}

FSituationalSound* UOmochaSoundDataAsset::FindSituationalSound(FGameplayTag LevelTag, FGameplayTag SituationTag)
{
	if (!SituationTag.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("FindSituationalSound: Invalid SituationTag"));
		return nullptr;
	}

	FSituationalSound* BestMatch = nullptr;
	int32 HighestPriority = -1;

	FLevelSoundData* LevelData = nullptr;
	if (LevelTag.IsValid())
	{
		LevelData = FindLevelSoundData(LevelTag);
	}

	if (LevelData) 
	{
		for (FSituationalSound& SitSound : LevelData->SituationalSounds) {
			if (SitSound.SituationTag.MatchesTag(SituationTag) && SitSound.Priority > HighestPriority) {
				BestMatch = &SitSound;
				HighestPriority = SitSound.Priority;
			}
		}
	}

	// Then check global situational sounds (if level allows it or no level specified)
	const bool bCheckGlobal = !LevelData || LevelData->bUseGlobalSituationalSounds;
	if (bCheckGlobal) 
	{
		for (FSituationalSound& GlobalSound : GlobalSituationalSounds) {
			if (GlobalSound.SituationTag.MatchesTag(SituationTag) && GlobalSound.Priority > HighestPriority) {
				BestMatch = &GlobalSound;
				HighestPriority = GlobalSound.Priority;
			}
		}
	}

	if (!BestMatch) {
		UE_LOG(LogTemp, Warning, TEXT("FindSituationalSound: No situational sound found for Level: %s, Situation: %s"),
		       *LevelTag.ToString(), *SituationTag.ToString());
	}

	return BestMatch;
}

TArray<FGameplayTag> UOmochaSoundDataAsset::GetAllLevelTags() const
{
	TArray<FGameplayTag> LevelTags;
	for (const FLevelSoundMapping& Mapping : LevelSoundMappings) {
		if (Mapping.LevelTag.IsValid()) {
			LevelTags.Add(Mapping.LevelTag);
		}
	}
	return LevelTags;
}

TArray<FGameplayTag> UOmochaSoundDataAsset::GetSituationTagsForLevel(FGameplayTag LevelTag) const
{
	TArray<FGameplayTag> SituationTags;

	// Add global situational sounds
	for (const FSituationalSound& GlobalSound : GlobalSituationalSounds) {
		if (GlobalSound.SituationTag.IsValid()) {
			SituationTags.AddUnique(GlobalSound.SituationTag);
		}
	}

	// Add level-specific situational sounds
	if (LevelTag.IsValid()) {
		for (const FLevelSoundMapping& Mapping : LevelSoundMappings) {
			if (Mapping.LevelTag.MatchesTagExact(LevelTag)) {
				for (const FSituationalSound& SitSound : Mapping.SoundData.SituationalSounds) {
					if (SitSound.SituationTag.IsValid()) {
						SituationTags.AddUnique(SitSound.SituationTag);
					}
				}
				break;
			}
		}
	}

	return SituationTags;
}

bool UOmochaSoundDataAsset::ValidateSetup(TArray<FString>& OutErrorMessages) const
{
	OutErrorMessages.Empty();
	bool bIsValid = true;

	// Check for duplicate level tags
	TSet<FGameplayTag> UsedLevelTags;
	for (const FLevelSoundMapping& Mapping : LevelSoundMappings) {
		if (!Mapping.LevelTag.IsValid()) {
			OutErrorMessages.Add(FString::Printf(TEXT("Invalid level tag in mapping: %s"), *Mapping.DisplayName));
			bIsValid = false;
			continue;
		}

		if (UsedLevelTags.Contains(Mapping.LevelTag)) {
			OutErrorMessages.Add(FString::Printf(TEXT("Duplicate level tag: %s"), *Mapping.LevelTag.ToString()));
			bIsValid = false;
		}
		else {
			UsedLevelTags.Add(Mapping.LevelTag);
		}

		// Check if default BGM is assigned
		if (!Mapping.SoundData.DefaultBGM) {
			OutErrorMessages.Add(
				FString::Printf(TEXT("No default BGM assigned for level: %s"), *Mapping.LevelTag.ToString()));
			// This is a warning, not an error
		}
	}

	// Check global situational sounds
	for (const FSituationalSound& GlobalSound : GlobalSituationalSounds) {
		if (!GlobalSound.SituationTag.IsValid()) {
			OutErrorMessages.Add(TEXT("Invalid situation tag in global situational sounds"));
			bIsValid = false;
		}

		if (!GlobalSound.SoundAsset) {
			OutErrorMessages.Add(FString::Printf(TEXT("No sound asset assigned for global situation: %s"),
			                                     *GlobalSound.SituationTag.ToString()));
		}
	}

	// Check level-specific situational sounds
	for (const FLevelSoundMapping& Mapping : LevelSoundMappings) {
		for (const FSituationalSound& LevelSound : Mapping.SoundData.SituationalSounds) {
			if (!LevelSound.SituationTag.IsValid()) {
				OutErrorMessages.Add(FString::Printf(TEXT("Invalid situation tag in level '%s' situational sounds"),
				                                     *Mapping.LevelTag.ToString()));
				bIsValid = false;
			}

			if (!LevelSound.SoundAsset) {
				OutErrorMessages.Add(FString::Printf(TEXT("No sound asset assigned for situation '%s' in level '%s'"),
				                                     *LevelSound.SituationTag.ToString(),
				                                     *Mapping.LevelTag.ToString()));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SoundDataAsset validation completed. Valid: %s, Errors: %d"),
	       bIsValid ? TEXT("Yes") : TEXT("No"), OutErrorMessages.Num());

	return bIsValid;
}
