// Copyright It's My Turn
#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/AudioComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "DataAsset/OmochaSoundDataAsset.h"
#include "SoundManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOmochaSound, Log, All);

USTRUCT(BlueprintType)
struct FSoundSystemConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UOmochaSoundDataAsset> SoundDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundClass> MasterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundClass> MusicClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundClass> SFXClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundClass> VoiceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundMix> MainMix;

	FSoundSystemConfig()
	{
		SoundDataAsset = nullptr;
		MasterClass = nullptr;
		MusicClass = nullptr;
		SFXClass = nullptr;
		VoiceClass = nullptr;
		MainMix = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSoundConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* SoundAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FadeInDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FadeOutDuration = 0.0f;

	FSoundConfig()
	{
		SoundAsset = nullptr;
		Volume = 1.0f;
		FadeInDuration = 0.0f;
		FadeOutDuration = 0.0f;
	}
};

/**
 * Enhanced SoundManager with unified config system
 */
UCLASS()
class OMOCHA_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Subsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Get singleton instance
	UFUNCTION(BlueprintCallable)
	static USoundManager* Get(const UObject* WorldContext);

	// Sound system initialization
	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	void InitializeSoundSystem(const FSoundSystemConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "SoundManager")
	bool IsSoundSystemInitialized() const;

	// Basic BGM functions
	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* Sound, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable)
	void StopBGM();

	UFUNCTION(BlueprintCallable)
	void PauseBGM();

	UFUNCTION(BlueprintCallable)
	void ResumeBGM();

	// Volume control
	UFUNCTION(BlueprintCallable)
	void SetBGMVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	float GetBGMVolume() const;

	// Enhanced level change functions
	UFUNCTION(BlueprintCallable)
	void OnLevelChanged(FGameplayTag LevelTag, bool bForceReplay = false);

	// Enhanced situational BGM change
	UFUNCTION(BlueprintCallable)
	void ChangeBGMForSituation(FGameplayTag SituationTag);

	UFUNCTION(BlueprintCallable)
	void EndCurrentSituation();

	// Sound Class volume control
	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetVoiceVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable)
	float GetMasterVolume() const;

	UFUNCTION(BlueprintCallable)
	float GetSFXVolume() const;

	UFUNCTION(BlueprintCallable)
	float GetVoiceVolume() const;

	UFUNCTION(BlueprintCallable)
	float GetMusicVolume() const;

	// Apply all sound settings
	UFUNCTION(BlueprintCallable)
	void ApplySoundSettings();

	// Future functions for fade effects
	UFUNCTION(BlueprintCallable)
	void FadeInBGM(USoundBase* Sound, float Duration = 2.0f);

	UFUNCTION(BlueprintCallable)
	void FadeOutBGM(float Duration = 2.0f);

	UFUNCTION(BlueprintCallable)
	void CrossFadeBGM(USoundBase* NewSound, float Duration = 2.0f);

	// Debug and validation functions
	UFUNCTION(BlueprintCallable)
	bool ValidateSoundDataAsset();

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentLevelTag() const { return CurrentLevelTag; }

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentSituationTag() const { return CurrentSituationTag; }

protected:
	// Sound system configuration (replaces all individual settings)
	UPROPERTY()
	FSoundSystemConfig CurrentConfig;

	// Current state tracking
	UPROPERTY()
	FGameplayTag CurrentLevelTag;

	UPROPERTY()
	FGameplayTag CurrentSituationTag;

	// Current BGM audio component
	UPROPERTY()
	UAudioComponent* CurrentBGMComponent;

	// Audio component that is currently fading out
	UPROPERTY()
	UAudioComponent* FadingOutBGMComponent;

	// Timer handle for the fade out cleanup
	FTimerHandle FadeOutTimerHandle;

	// Called by the timer when the fade out is complete
	UFUNCTION()
	void OnFadeOutComplete();

	// BGM history for restoration
	UPROPERTY()
	FSoundConfig LastLevelBGMConfig;

	// Current BGM config
	UPROPERTY()
	FSoundConfig CurrentBGMConfig;

	// Current volume settings
	UPROPERTY()
	float CurrentMasterVolume = 1.0f;

	UPROPERTY()
	float CurrentSFXVolume = 1.0f;

	UPROPERTY()
	float CurrentVoiceVolume = 1.0f;

	UPROPERTY()
	float CurrentMusicVolume = 1.0f;

	// Default settings
	UPROPERTY(EditDefaultsOnly, Category = "Default Sound Settings")
	float DefaultMasterVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Default Sound Settings")
	float DefaultMusicVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Default Sound Settings")
	float DefaultSFXVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Default Sound Settings")
	float DefaultVoiceVolume = 1.0f;

	// Initialization state
	UPROPERTY()
	bool bIsSoundSystemInitialized = false;

	UFUNCTION()
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

	UFUNCTION()
	void OnPreWorldFinishDestroy(UWorld* World);

	// Safe cleanup function
	void SafeCleanupAllAudio();

private:
	// Config helper functions (replaces direct member access)
	UOmochaSoundDataAsset* GetSoundDataAsset() const;
	USoundClass* GetMasterSoundClass() const;
	USoundClass* GetMusicSoundClass() const;
	USoundClass* GetSFXSoundClass() const;
	USoundClass* GetVoiceSoundClass() const;
	USoundMix* GetMainSoundMix() const;

	// Internal helper functions
	UAudioComponent* CreateAudioComponent();
	void CleanupAudioComponent(UAudioComponent* Component);
	void SetSoundClassVolume(USoundClass* SoundClass, float Volume);

	// Validation functions
	bool IsValidConfigSetup() const;

	// Helper function for safe World access
	UWorld* GetValidWorld() const;
};
