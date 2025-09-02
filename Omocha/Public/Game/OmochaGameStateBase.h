// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OmochaGameStateBase.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoteCountChanged, int32, CurrentVotes, int32, RequiredVotes);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkipCinematicApproved);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamLevelChanged, int32, NewLevel, int32, OldLevel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTeamXPChanged, float, CurrentXP, float, RequiredXP, float, XPPercent);

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaGameStateBase : public AGameState
{
	GENERATED_BODY()

public:
	AOmochaGameStateBase();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void StartCinematicSkipVoting();

	UFUNCTION(BlueprintCallable)
	void AddCinematicSkipVote();

	UFUNCTION(BlueprintCallable)
	void RemoveCinematicSkipVote();

	UFUNCTION(BlueprintCallable)
	void EndCinematicSkipVoting();

	UFUNCTION(BlueprintPure)
	void GetVoteStatus(int32& CurrentVotes, int32& RequiredVotes, bool& bIsActive) const;

	UFUNCTION(BlueprintCallable, Category = "Voting")
	int32 GetCurrentPlayerCount() const;

	UFUNCTION(BlueprintCallable, Category = "Voting")
	void UpdateRequiredVotes();

	UPROPERTY(BlueprintAssignable)
	FOnVoteCountChanged OnVoteCountChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSkipCinematicApproved OnSkipCinematicApproved;

	// Level System
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TeamLevel, Category = "Team Level")
	int32 TeamLevel = 1;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TeamXP, Category = "Team Level")
	float TeamXP = 0;

	// Required Experience Values by Level
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level System")
	TArray<int32> XPRequirements;

	UPROPERTY(BlueprintAssignable, Category = "Team Level")
	FOnTeamLevelChanged OnTeamLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Team Level")
	FOnTeamXPChanged OnTeamXPChanged;

	UFUNCTION(BlueprintPure, Category = "Level System")
	int32 GetXPRequiredForLevel(int32 Level) const;

	UFUNCTION(BlueprintPure, Category = "Level System")
	int32 GetXPRequiredForNextLevel() const;

	UFUNCTION(BlueprintPure, Category = "Level System")
	float GetXPPercent() const;
	
	UFUNCTION(BlueprintCallable, Category = "LevelSystem", CallInEditor)
	void AddTeamXP(float XPAmount);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSkipVotes)
	int32 CurrentSkipVotes;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredSkipVotes = 3;

	UPROPERTY(ReplicatedUsing = OnRep_VotingActive)
	bool bSkipVotingActive = false;

	UFUNCTION()
	void OnRep_CurrentSkipVotes();

	UFUNCTION()
	void OnRep_VotingActive();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level System")
	TSubclassOf<UGameplayEffect> SetLevelGEClass;

private:
	void CheckVoteResult();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnSkipApproved();

	void InitializeFromGameInstance();
	void CheckLevelUp();
	void UpdateAllPlayerXPAndLevel();
	void UpdatePlayersLevel(UAbilitySystemComponent* ASC, int32 NewLevel);
	void BackupToGameInstance();
	void InitializeXPTable();
	void BroadcastXPChange();

	UFUNCTION()
	void OnRep_TeamLevel(int32 OldLevel);

	UFUNCTION()
	void OnRep_TeamXP(float OldXP);
};
