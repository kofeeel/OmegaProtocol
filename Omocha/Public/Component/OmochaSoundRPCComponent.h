// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "OmochaSoundRPCComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UOmochaSoundRPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOmochaSoundRPCComponent();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_UpdateGlobalBGM(FGameplayTag NewSoundTag);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_UpdateGlobalBGM(FGameplayTag NewSoundTag);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_PlaySituationalSound(FGameplayTag SituationTag);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_PlaySituationalSound(FGameplayTag SituationTag);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_InitializeSoundSystem(const FSoundSystemConfig& Config, FGameplayTag InitialLevelTag);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_EndSituationalSound();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_EndSituationalSound();
};
