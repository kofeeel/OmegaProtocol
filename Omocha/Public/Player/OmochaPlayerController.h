// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/SkillBuildData.h"
#include "Game/OmochaScoreBoardGameMode.h"
#include "GameFramework/PlayerController.h"
#include "OmochaPlayerController.generated.h"

struct FGameScoreBoardData;
class UOmochaSessionSubsystem;
enum class ECharacterType : uint8;
class AOmochaPlayerCharacter;
class UOmochaAbilitySystemComponent;
class UOmochaInputConfig;
struct FGameplayTag;
class UInputMappingContext;
class UInputAction;
class UOmochaSoundRPCComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerEventBoolAlram, bool, bReady);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChatMessageArribed, const FString& Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreBoardDataResponse, const FGameScoreBoardData&, Data);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerLogin, int32, PlayerNumber, const FString&, PlayerName,
                                               const FString&, SteamId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCharacter, int32, PlayerNumber, const ECharacterType&,
                                             CharacterType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReady, int32, PlayerNumber, const bool, Ready);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerRestartReady, int32, PlayerNumber, const EPlayerRestartState&,
                                             State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifySignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FBuildInfoDelegate, const FSkillBuildInfos& Info);


/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOmochaPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look System")
	bool bEnableLookSystem = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look System")
	float MinLookDistance = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look System")
	float MaxSpineRotation = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look System")
	float SpineRotationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look System")
	float BodyRotationSpeed = 3.0f;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* Target, bool bBlockedHit = false, bool bCriticalHit = false);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ShowDamageNumberGC(float DamageAmount, ACharacter* Target, bool bBlockedHit, bool bCriticalHit);
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetInputBlocked(bool bBlocked);

	UFUNCTION(BlueprintCallable, Category = "Cursor")
	FHitResult GetMouseCursorHit() const { return CursorHit; }

	/* GameMode */
	UFUNCTION(Client, Reliable)
	void Client_GetPrepareForNextLevel(bool bReady);
	UFUNCTION(Server, Reliable)
	void Server_ChoosePlayerCharacter(const FString& PlayerId, const ECharacterType CharacterType);
	UFUNCTION(Server, Reliable)
	void Server_PlayerGetReady(const FString& PlayerId, const bool Ready);
	UFUNCTION(Client, Reliable)
	void Client_PlayerLogin(int32 PlayerNum, const FString& PlayerName, const FString& SteamId);
	UFUNCTION(Client, Reliable)
	void Client_PlayerCharacterChanged(int32 PlayerNum, const ECharacterType& CharacterType);
	UFUNCTION(Client, Reliable)
	void Client_PlayerReady(int32 PlayerNum, const bool Ready);
	UFUNCTION(Server, Reliable)
	void Server_InitializeRequest();
	UFUNCTION(Client, Reliable)
	void Client_InitializeResponse(int32 PlayerNum, const FString& PlayerName, const FString& SteamId,
	                               const ECharacterType& CharacterType, const bool Ready);
	UFUNCTION(BlueprintCallable)
	bool IsLocallyServerController(int32 PlayerNumber);
	UFUNCTION(Client, Reliable)
	void Client_GameOver(const bool Success);
	UFUNCTION(Server, Reliable)
	void Server_ScoreDataRequest();
	UFUNCTION(Client, Reliable)
	void Client_ScoreDataResponse(const FGameScoreBoardData& Data);
	UFUNCTION(Client, Reliable)
	void Client_PlayerRestartState(int32 PlayerNumber, const EPlayerRestartState& State);
	UFUNCTION(Server, Reliable)
	void Server_PlayerRestartReady(const bool bReady);

	UFUNCTION(Client, Reliable)
	void Client_StartPreTravelEffects(float FadeDuration);

	UFUNCTION(BlueprintCallable)
	void SetNextLevel(const FString& MapName);
	UFUNCTION(BlueprintCallable)
	void ReadyForNextLevel(const bool Ready, const bool Save);
	UFUNCTION(BlueprintCallable)
	void KickPlayer(int32 PlayerNum);

	UPROPERTY(BlueprintAssignable)
	FOnServerEventBoolAlram OnPrepareNextLevel;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerLogin OnPlayerLogin;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerCharacter OnPlayerCharacter;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerReady OnPlayerReady;
	UPROPERTY(BlueprintAssignable)
	FOnServerEventBoolAlram OnGameOver;
	UPROPERTY(BlueprintAssignable)
	FOnScoreBoardDataResponse OnScoreBoardDataResponse;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerRestartReady OnScoreBoardPlayerReady;
	/* GameMode End */

	void BindingInputAction();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UOmochaSoundRPCComponent> SoundRPCComponent;

	// Player View Change
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spectator UI")
	TSubclassOf<class USpectatorWidget> SpectatorWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Spectator UI")
	class USpectatorWidget* SpectatorWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Spectator")
	bool bIsSpectating = false;

	UPROPERTY(BlueprintReadOnly, Category = "Spectator")
	TArray<APawn*> SpectatorTargets;

	UPROPERTY(BlueprintReadOnly, Category = "Spectator")
	int32 CurrentSpectatorIndex = 0;

	UFUNCTION(BlueprintCallable, Category = "Spectator")
	void StartSpectating();

	UFUNCTION(BlueprintCallable, Category = "Spectator")
	void StopSpectating();

	UFUNCTION(Client, Reliable)
	void Client_StopSpectating();

	UFUNCTION(BlueprintCallable, Category = "Spectator")
	void NextSpectatorTarget();

	UFUNCTION(BlueprintCallable, Category = "Spectator")
	void RefreshSpectatorTargets();

	UFUNCTION(Client, Reliable)
	void Client_StartSpectating(const TArray<APawn*>& AvailableTargets);

	UFUNCTION(Client, Reliable)
	void Client_UpdateSpectatorTargets(const TArray<APawn*>& AvailableTargets);

	void HandleSpectatorNext();

	UFUNCTION(BlueprintPure, Category = "Spectator")
	bool IsSpectating() const { return bIsSpectating; }

	UFUNCTION(BlueprintPure, Category = "Spectator")
	FString GetCurrentSpectatorTargetName() const;

	UFUNCTION(BlueprintPure, Category = "Spectator")
	int32 GetCurrentSpectatorIndex() const { return CurrentSpectatorIndex; }

	UFUNCTION(BlueprintPure, Category = "Spectator")
	int32 GetSpectatorTargetsNum() const { return SpectatorTargets.Num(); }

	void SetCursorEnabled(bool bCursorEnabled);

	/* Chatting Service */
	FOnChatMessageArribed OnChatMessageArrived;

	UFUNCTION(Server, Reliable)
	void Server_SendChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void Client_AddChatMessage(const FString& Message);
	/* End Chatting */

	UFUNCTION(Exec)
	void AddXP(int32 Amount);

	UFUNCTION(Exec)
	void ShowMyAttributes();

	UFUNCTION(Exec)
	void ShowTeamLevel();

	UFUNCTION(Exec)
	void ShowPersistent();

	UFUNCTION(Exec)
	void AddBuild(const FString& BuildTagString);

	/* Build Actor Used Properties */
	UFUNCTION(BlueprintCallable)
	void AddBuildByTag(const FGameplayTag& BuildTag);

	FBuildInfoDelegate BuildInfoDelegate;
	
	UFUNCTION(BlueprintCallable)
	void ChooseBuilds(const FSkillBuildInfos& Infos);

	FOnNotifySignature BuildPickedDelegate;

	UFUNCTION(BlueprintCallable)
	void PickBuild();

	UPROPERTY(BlueprintAssignable)
	FOnNotifySignature BuildCardCloseDelegate;

	UFUNCTION(BlueprintCallable)
	void CloseBuilds();

	/* End Build Actor Used Properties */
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void Interact();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UOmochaDamageTextComponent> DamageTextComponentClass;

	void ShowStatWidget();
	void HideStatWidget();

private:
	UPROPERTY()
	TObjectPtr<UOmochaAbilitySystemComponent> AbilitySystemComponent;

	void ShowEffectItemToolTip(AActor* EffectItem);
	void HideEffectItemToolTip(AActor* EffectItem);

	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;
	FHitResult CursorHit;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> Context;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PlayerViewChangeAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UOmochaInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleStatsAction;

	UOmochaAbilitySystemComponent* GetASC();


	// Look System Settings

	float CurrentSpineRotation = 0.0f;

	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace();

	void AbilityInputTagPressed(FGameplayTag InputTag);

	void AbilityInputTagReleased(FGameplayTag InputTag);

	void AbilityInputTagHeld(FGameplayTag InputTag);

	// Look System
	void UpdateLookSystem(float DeltaTime);
	FVector GetMouseWorldPosition() const;

	// Utility Functions
	static float NormalizeAngle(float Angle);
	static float AngleDifference(float From, float To);
	static float LerpAngle(float Current, float Target, float DeltaTime, float Speed);

	bool bInputCurrentlyBlocked = false;

	// Player View Change(Die)
	void OnSpectatorNext();
	void CreateSpectatorUI();
};
