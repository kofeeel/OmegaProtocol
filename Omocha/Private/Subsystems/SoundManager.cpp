// Copyright It's My Turn

#include "Omocha/Public/Subsystems/SoundManager.h"
#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

DEFINE_LOG_CATEGORY(LogOmochaSound);

void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentBGMComponent = nullptr;
	CurrentBGMConfig = FSoundConfig();
	LastLevelBGMConfig = FSoundConfig();
	CurrentLevelTag = FGameplayTag::EmptyTag;
	CurrentSituationTag = FGameplayTag::EmptyTag;
	CurrentMasterVolume = DefaultMasterVolume;
	CurrentSFXVolume = DefaultSFXVolume;
	CurrentVoiceVolume = DefaultVoiceVolume;
	CurrentMusicVolume = DefaultMusicVolume;
	bIsSoundSystemInitialized = false;

	FWorldDelegates::OnWorldCleanup.AddUObject(this, &USoundManager::OnWorldCleanup);
	FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &USoundManager::OnPreWorldFinishDestroy);
}

void USoundManager::Deinitialize()
{
	FWorldDelegates::OnWorldCleanup.RemoveAll(this);
	FWorldDelegates::OnPreWorldFinishDestroy.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	SafeCleanupAllAudio();
	bIsSoundSystemInitialized = false;

	Super::Deinitialize();
}

USoundManager* USoundManager::Get(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull)) {
		if (UGameInstance* GameInstance = World->GetGameInstance()) {
			return GameInstance->GetSubsystem<USoundManager>();
		}
	}
	return nullptr;
}

void USoundManager::InitializeSoundSystem(const FSoundSystemConfig& Config)
{
	CurrentConfig = Config;

	if (!IsValidConfigSetup()) 
	{
		UE_LOG(LogOmochaSound, Error, TEXT("InitializeSoundSystem: Config validation failed AFTER loading asset."));
		return;
	}

	bIsSoundSystemInitialized = true;
	ApplySoundSettings();
}

bool USoundManager::IsSoundSystemInitialized() const
{
	return bIsSoundSystemInitialized && GetSoundDataAsset() != nullptr;
}

void USoundManager::PlayBGM(USoundBase* Sound, float Volume)
{
	if (!Sound || !IsSoundSystemInitialized()) {
		return;
	}

	StopBGM();

	CurrentBGMComponent = CreateAudioComponent();
	if (!CurrentBGMComponent) {
		return;
	}

	CurrentBGMComponent->SetSound(Sound);
	CurrentBGMComponent->SetVolumeMultiplier(Volume);
	CurrentBGMComponent->bAutoDestroy = false;

	CurrentBGMConfig.SoundAsset = Sound;
	CurrentBGMConfig.Volume = Volume;

	CurrentBGMComponent->Play();
}

void USoundManager::StopBGM()
{
	if (GetWorld() && FadeOutTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
	}

	if (CurrentBGMComponent) {
		CleanupAudioComponent(CurrentBGMComponent);
		CurrentBGMComponent = nullptr;
	}

	if (FadingOutBGMComponent) {
		CleanupAudioComponent(FadingOutBGMComponent);
		FadingOutBGMComponent = nullptr;
	}

	CurrentBGMConfig = FSoundConfig();
}

void USoundManager::PauseBGM()
{
	if (CurrentBGMComponent && CurrentBGMComponent->IsPlaying()) {
		CurrentBGMComponent->SetPaused(true);
	}
}

void USoundManager::ResumeBGM()
{
	if (CurrentBGMComponent) {
		CurrentBGMComponent->SetPaused(false);
	}
}

void USoundManager::SetBGMVolume(float Volume)
{
	if (CurrentBGMComponent) {
		CurrentBGMComponent->SetVolumeMultiplier(Volume);
		CurrentBGMConfig.Volume = Volume;
	}
}

float USoundManager::GetBGMVolume() const
{
	return CurrentBGMConfig.Volume;
}

void USoundManager::OnLevelChanged(FGameplayTag LevelTag, bool bForceReplay)
{
	if (!LevelTag.IsValid() || !IsSoundSystemInitialized()) {
		return;
	}

	if (!bForceReplay && CurrentLevelTag.MatchesTagExact(LevelTag)) {
		return;
	}

	LastLevelBGMConfig = FSoundConfig();
	CurrentSituationTag = FGameplayTag::EmptyTag;
	CurrentLevelTag = LevelTag;

	UOmochaSoundDataAsset* DataAsset = GetSoundDataAsset();
	if (!DataAsset) {
		return;
	}

	constexpr float DefaultFadeDuration = 2.0f;

	FLevelSoundData* LevelData = DataAsset->FindLevelSoundData(LevelTag);
	if (!LevelData) {
		if (DataAsset->FallbackBGM) {
			CrossFadeBGM(DataAsset->FallbackBGM, DefaultFadeDuration);
		}
		else {
			FadeOutBGM(DefaultFadeDuration);
		}
		return;
	}

	if (LevelData->DefaultBGM) {
		CrossFadeBGM(LevelData->DefaultBGM, DefaultFadeDuration);
	}
	else if (DataAsset->FallbackBGM) {
		CrossFadeBGM(DataAsset->FallbackBGM, DefaultFadeDuration);
	}
	else {
		FadeOutBGM(DefaultFadeDuration);
	}
}

void USoundManager::ChangeBGMForSituation(FGameplayTag SituationTag)
{
	if (!SituationTag.IsValid() || !IsSoundSystemInitialized()) {
		return;
	}

	if (CurrentSituationTag.MatchesTagExact(SituationTag)) {
		return;
	}

	if (!CurrentSituationTag.IsValid() && CurrentBGMComponent && CurrentBGMConfig.SoundAsset) {
		LastLevelBGMConfig = CurrentBGMConfig;
	}

	CurrentSituationTag = SituationTag;

	UOmochaSoundDataAsset* DataAsset = GetSoundDataAsset();
	if (!DataAsset) {
		return;
	}

	FSituationalSound* SituationalSound = DataAsset->FindSituationalSound(CurrentLevelTag, SituationTag);
	if (!SituationalSound || !SituationalSound->SoundAsset) {
		CurrentSituationTag = FGameplayTag::EmptyTag;
		return;
	}

	constexpr float DefaultFadeDuration = 1.0f;

	CrossFadeBGM(SituationalSound->SoundAsset, DefaultFadeDuration);
}

void USoundManager::EndCurrentSituation()
{
	if (!CurrentSituationTag.IsValid()) {
		return;
	}

	CurrentSituationTag = FGameplayTag::EmptyTag;
	constexpr float DefaultFadeDuration = 2.0f;

	if (LastLevelBGMConfig.SoundAsset) {
		CrossFadeBGM(LastLevelBGMConfig.SoundAsset, DefaultFadeDuration);
	}
	else {
		OnLevelChanged(CurrentLevelTag, true);
	}
}

void USoundManager::SetMasterVolume(float Volume)
{
	CurrentMasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	if (USoundClass* MasterClass = GetMasterSoundClass()) {
		SetSoundClassVolume(MasterClass, CurrentMasterVolume);
	}
}

void USoundManager::SetSFXVolume(float Volume)
{
	CurrentSFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	if (USoundClass* SFXClass = GetSFXSoundClass()) {
		SetSoundClassVolume(SFXClass, CurrentSFXVolume);
	}
}

void USoundManager::SetVoiceVolume(float Volume)
{
	CurrentVoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	if (USoundClass* VoiceClass = GetVoiceSoundClass()) {
		SetSoundClassVolume(VoiceClass, CurrentVoiceVolume);
	}
}

void USoundManager::SetMusicVolume(float Volume)
{
	CurrentMusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	if (USoundClass* MusicClass = GetMusicSoundClass()) {
		SetSoundClassVolume(MusicClass, CurrentMusicVolume);
	}
}

float USoundManager::GetMasterVolume() const { return CurrentMasterVolume; }
float USoundManager::GetSFXVolume() const { return CurrentSFXVolume; }
float USoundManager::GetVoiceVolume() const { return CurrentVoiceVolume; }
float USoundManager::GetMusicVolume() const { return CurrentMusicVolume; }

void USoundManager::ApplySoundSettings()
{
	if (!IsSoundSystemInitialized()) {
		return;
	}

	SetMasterVolume(CurrentMasterVolume);
	SetSFXVolume(CurrentSFXVolume);
	SetVoiceVolume(CurrentVoiceVolume);
	SetMusicVolume(CurrentMusicVolume);
}

void USoundManager::FadeInBGM(USoundBase* Sound, float Duration)
{
	if (!Sound || !IsSoundSystemInitialized()) {
		return;
	}

	StopBGM();

	CurrentBGMComponent = CreateAudioComponent();
	if (!CurrentBGMComponent) {
		return;
	}

	CurrentBGMComponent->SetSound(Sound);
	CurrentBGMComponent->FadeIn(Duration);

	CurrentBGMConfig.SoundAsset = Sound;
}

void USoundManager::FadeOutBGM(float Duration)
{
	if (!CurrentBGMComponent || !CurrentBGMComponent->IsPlaying() || !GetWorld()) {
		return;
	}

	if (FadeOutTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
		OnFadeOutComplete();
	}

	FadingOutBGMComponent = CurrentBGMComponent;
	CurrentBGMComponent = nullptr;

	FadingOutBGMComponent->FadeOut(Duration, 0.0f);

	GetWorld()->GetTimerManager().SetTimer(
		FadeOutTimerHandle,
		this,
		&USoundManager::OnFadeOutComplete,
		Duration,
		false);

	CurrentBGMConfig = FSoundConfig();
}

void USoundManager::CrossFadeBGM(USoundBase* NewSound, float Duration)
{
	if (!NewSound || !IsSoundSystemInitialized()) {
		return;
	}

	if (CurrentBGMComponent && CurrentBGMComponent->IsPlaying()) {
		FadeOutBGM(Duration);
	}

	CurrentBGMComponent = CreateAudioComponent();
	if (!CurrentBGMComponent) {
		return;
	}

	CurrentBGMComponent->SetSound(NewSound);
	CurrentBGMComponent->FadeIn(Duration);

	CurrentBGMConfig.SoundAsset = NewSound;
	CurrentBGMConfig.Volume = 1.0f;
}

bool USoundManager::ValidateSoundDataAsset()
{
	UOmochaSoundDataAsset* DataAsset = GetSoundDataAsset();
	if (!DataAsset) {
		return false;
	}

	TArray<FString> ErrorMessages;
	return DataAsset->ValidateSetup(ErrorMessages);
}

void USoundManager::OnFadeOutComplete()
{
	if (FadingOutBGMComponent) {
		CleanupAudioComponent(FadingOutBGMComponent);
		FadingOutBGMComponent = nullptr;
	}
}

UOmochaSoundDataAsset* USoundManager::GetSoundDataAsset() const
{
	return CurrentConfig.SoundDataAsset.Get();
}

USoundClass* USoundManager::GetMasterSoundClass() const
{
	return CurrentConfig.MasterClass.Get();
}

USoundClass* USoundManager::GetMusicSoundClass() const
{
	return CurrentConfig.MusicClass.Get();
}

USoundClass* USoundManager::GetSFXSoundClass() const
{
	return CurrentConfig.SFXClass.Get();
}

USoundClass* USoundManager::GetVoiceSoundClass() const
{
	return CurrentConfig.VoiceClass.Get();
}

USoundMix* USoundManager::GetMainSoundMix() const
{
	return CurrentConfig.MainMix.Get();
}

UAudioComponent* USoundManager::CreateAudioComponent()
{
	UWorld* World = GetValidWorld();
	if (!World) {
		return nullptr;
	}

	UAudioComponent* AudioComp = NewObject<UAudioComponent>(
		World,
		UAudioComponent::StaticClass(),
		NAME_None,
		RF_Transient
	);

	if (AudioComp) {
		AudioComp->bAutoDestroy = false;
		AudioComp->bStopWhenOwnerDestroyed = false;
		AudioComp->bIsUISound = true;
		AudioComp->bAllowSpatialization = false;
		AudioComp->bAutoActivate = false;
		AudioComp->RegisterComponentWithWorld(World);
		return AudioComp;
	}

	return nullptr;
}

void USoundManager::CleanupAudioComponent(UAudioComponent* Component)
{
	if (!Component || !IsValid(Component)) {
		return;
	}

	if (Component->IsPlaying()) {
		Component->FadeOut(0.1f, 0.0f);
	}

	Component->SetSound(nullptr);
	Component->OnAudioFinished.Clear();
	Component->OnAudioPlaybackPercent.Clear();
	Component->SetActive(false);

	if (Component->IsRegistered()) {
		Component->UnregisterComponent();
	}

	Component->DestroyComponent();
}

void USoundManager::SetSoundClassVolume(USoundClass* SoundClass, float Volume)
{
	if (!SoundClass) {
		return;
	}

	USoundMix* MainMix = GetMainSoundMix();
	if (!MainMix) {
		return;
	}

	UWorld* World = GetValidWorld();
	if (!World) {
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(World, MainMix, SoundClass, Volume, 1.0f, 0.1f);
	UGameplayStatics::PushSoundMixModifier(World, MainMix);
}

bool USoundManager::IsValidConfigSetup() const
{
	UOmochaSoundDataAsset* DataAsset = GetSoundDataAsset();
	if (!DataAsset) {
		return false;
	}

	TArray<FString> ErrorMessages;
	return DataAsset->ValidateSetup(ErrorMessages);
}

void USoundManager::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	if (World == GetValidWorld()) {
		SafeCleanupAllAudio();
	}
}

void USoundManager::OnPreWorldFinishDestroy(UWorld* World)
{
	if (World == GetValidWorld()) {
		SafeCleanupAllAudio();
	}
}

void USoundManager::SafeCleanupAllAudio()
{
	if (CurrentBGMComponent) {
		CleanupAudioComponent(CurrentBGMComponent);
		CurrentBGMComponent = nullptr;
	}

	if (FadingOutBGMComponent) {
		CleanupAudioComponent(FadingOutBGMComponent);
		FadingOutBGMComponent = nullptr;
	}

	CurrentBGMConfig = FSoundConfig();
	LastLevelBGMConfig = FSoundConfig();
}

UWorld* USoundManager::GetValidWorld() const
{
	if (UGameInstance* GameInstance = GetGameInstance()) {
		UWorld* World = GameInstance->GetWorld();
		if (IsValid(World)) {
			return World;
		}
	}
	return nullptr;
}
