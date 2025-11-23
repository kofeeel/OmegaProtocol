// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaGameInstance.h"
#include "GameFramework/GameMode.h"
#include "OmochaBaseGameMode.generated.h"

class UOmochaAttributeSet;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class CSteamID;

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AOmochaBaseGameMode();
	void SavePlayerInformation(APlayerState* PS, UAttributeSet* AS, UAbilitySystemComponent* ASC);
	void LoadPlayerInformation(APlayerState* PS, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	void TravelToMap(bool InbSave = false, bool InbInit = false);

	UFUNCTION(BlueprintCallable)
	void SetNextMap(const FString& MapName);

	void CancelTravel();

	void KickPlayer(int32 PlayerNum) const;

	void SendChatMessage(const FString& Message) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TravelTime = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Travel")
	float PreTravelFadeDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Loaded Attribute Class")
	TSubclassOf<UGameplayEffect> AttributesSetByCallerEffect;

protected:
	void KickPlayerWithReason(APlayerController* PlayerToKick, const FString& KickReason) const;

	void SaveEveryPlayerCharacter();

	void GetPrepareForNextLevel(bool bReady);

	void TravelNextLevel();

	void FinalizeTravel();

	FString TravelToWorld = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ScoreBoardMap = FString(TEXT("ScoreBoard"));

	FTimerHandle TravelTimerHandle;

private:
	void SavePlayerAttributes(const APlayerState* PS, const UAttributeSet* AS, UOmochaGameInstance* OmochaGameInstance);
	void SavePlayerAbilities(APlayerState* PS, UAbilitySystemComponent* ASC,
	                         UOmochaGameInstance* OmochaGameInstance) const;
	void SavePlayerState(APlayerState* PS, const UAttributeSet* AS, UAbilitySystemComponent* ASC,
	                     UOmochaGameInstance* OmochaGameInstance);
	void SavePlayerWeapon(APlayerState* PS, UAbilitySystemComponent* ASC, UOmochaGameInstance* OmochaGameInstance);
	void SavePlayerBuilds(APlayerState* PS, UOmochaGameInstance* OmochaGameInstance);

	void LoadPlayerAttributes(APlayerState* PS, UAbilitySystemComponent* ASC, UOmochaGameInstance* OmochaGameInstance);
	void LoadPlayerAbilities(APlayerState* PS, UAbilitySystemComponent* ASC, UOmochaGameInstance* OmochaGameInstance);
	void LoadPlayerState(APlayerState* PS, UAbilitySystemComponent* ASC, UOmochaGameInstance* OmochaGameInstance);
	void LoadPlayerBuilds(APlayerState* PS, UOmochaGameInstance* OmochaGameInstance);

	bool bSave = false;
	bool bInit = false;
};
