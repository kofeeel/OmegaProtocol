// Copyright It's My Turn


#include "Component/OmochaSoundRPCComponent.h"

#include "Omocha/Public/Subsystems/SoundManager.h"


UOmochaSoundRPCComponent::UOmochaSoundRPCComponent()
{
	SetIsReplicatedByDefault(true);
}

void UOmochaSoundRPCComponent::Client_UpdateGlobalBGM_Implementation(FGameplayTag NewSoundTag)
{
	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->OnLevelChanged(NewSoundTag, true);
	}
}

void UOmochaSoundRPCComponent::Multicast_UpdateGlobalBGM_Implementation(FGameplayTag NewSoundTag)
{
	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		UE_LOG(LogTemp, Warning, TEXT("Multicast_UpdateGlobalBGM called on client. Changing BGM."));
		SoundManager->OnLevelChanged(NewSoundTag, true);
	}
}

void UOmochaSoundRPCComponent::Multicast_PlaySituationalSound_Implementation(FGameplayTag SituationTag)
{
	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->ChangeBGMForSituation(SituationTag);
	}
}

void UOmochaSoundRPCComponent::Client_PlaySituationalSound_Implementation(FGameplayTag SituationTag)
{
	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->ChangeBGMForSituation(SituationTag);
	}
}

void UOmochaSoundRPCComponent::Client_InitializeSoundSystem_Implementation(
	const FSoundSystemConfig& Config, FGameplayTag InitialLevelTag)
{
	if (GetOwner() && !GetOwner()->IsA<APlayerController>()) {
		return;
	}
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->IsLocalController()) {
		return;
	}

	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->InitializeSoundSystem(Config);
		SoundManager->OnLevelChanged(InitialLevelTag, true);
	}
}

void UOmochaSoundRPCComponent::Client_EndSituationalSound_Implementation()
{
	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->EndCurrentSituation();
	}
}

void UOmochaSoundRPCComponent::Multicast_EndSituationalSound_Implementation()
{
	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->EndCurrentSituation();
	}
}
