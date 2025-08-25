// Copyright It's My Turn


#include "Game/OmochaGameStateBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Game/OmochaGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Player/OmochaPlayerState.h"

AOmochaGameStateBase::AOmochaGameStateBase()
{
	RequiredSkipVotes = 1;
	CurrentSkipVotes = 0;
	bSkipVotingActive = false;
}

void AOmochaGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	// XP Table Initialize
	InitializeXPTable();

	InitializeFromGameInstance();
}

void AOmochaGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOmochaGameStateBase, CurrentSkipVotes);
	DOREPLIFETIME(AOmochaGameStateBase, RequiredSkipVotes);
	DOREPLIFETIME(AOmochaGameStateBase, bSkipVotingActive);

	DOREPLIFETIME(AOmochaGameStateBase, TeamLevel);
	DOREPLIFETIME(AOmochaGameStateBase, TeamXP);
}

void AOmochaGameStateBase::StartCinematicSkipVoting()
{
	if (HasAuthority())
	{
		CurrentSkipVotes = 0;

		UpdateRequiredVotes();
		
		bSkipVotingActive = true;
	}
}

void AOmochaGameStateBase::AddCinematicSkipVote()
{
	if (HasAuthority() && bSkipVotingActive)
	{
		CurrentSkipVotes++;
		CheckVoteResult();
	}
}

void AOmochaGameStateBase::RemoveCinematicSkipVote()
{
	if (HasAuthority() && bSkipVotingActive && CurrentSkipVotes > 0)
	{
		CurrentSkipVotes--;
	}
}

void AOmochaGameStateBase::EndCinematicSkipVoting()
{
	if (HasAuthority())
	{
		CurrentSkipVotes = 0;
		bSkipVotingActive = false;
	}
}

void AOmochaGameStateBase::GetVoteStatus(int32& CurrentVotes, int32& RequiredVotes, bool& bIsActive) const
{
	CurrentVotes = CurrentSkipVotes;
	RequiredVotes = RequiredSkipVotes;
	bIsActive = bSkipVotingActive;
}

int32 AOmochaGameStateBase::GetCurrentPlayerCount() const
{
	int32 PlayerCount = 0;

	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState && !PlayerState->IsInactive())
		{
			PlayerCount++;
		}
	}

	return PlayerCount;
}

void AOmochaGameStateBase::UpdateRequiredVotes()
{
	if (HasAuthority())
	{
		int32 CurrentPlayerCount = GetCurrentPlayerCount();
		RequiredSkipVotes = FMath::Max(1, CurrentPlayerCount);
	}
}

void AOmochaGameStateBase::OnRep_CurrentSkipVotes()
{
	OnVoteCountChanged.Broadcast(CurrentSkipVotes, RequiredSkipVotes);
}

void AOmochaGameStateBase::OnRep_VotingActive()
{
	if (bSkipVotingActive)
	{
		OnVoteCountChanged.Broadcast(CurrentSkipVotes, RequiredSkipVotes);
	}
}

void AOmochaGameStateBase::CheckVoteResult()
{
	if (HasAuthority() && CurrentSkipVotes >= RequiredSkipVotes)
	{
		Multicast_OnSkipApproved();
		EndCinematicSkipVoting();
	}
}

void AOmochaGameStateBase::Multicast_OnSkipApproved_Implementation()
{
	OnSkipCinematicApproved.Broadcast();
}

void AOmochaGameStateBase::AddTeamXP(int32 XPAmount)
{
	if (!HasAuthority() || XPAmount <= 0) return;

	TeamXP += XPAmount;

	UE_LOG(LogTemp, Warning, TEXT("GameState: Team gained %d XP! Total: %d XP"), XPAmount, TeamXP);

	CheckLevelUp();

	BackupToGameInstance();

	BroadcastXPChange();
}

void AOmochaGameStateBase::InitializeFromGameInstance()
{
	if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		int32 MainTainLevel, MainTainXP;

		if (GI == nullptr)
		{
			return;
		}
		
		GI->GetMainTainLevelData(MainTainLevel, MainTainXP);

		TeamLevel = MainTainLevel;
		TeamXP = MainTainXP;

		UE_LOG(LogTemp, Warning, TEXT("GameState: Initialized from GameInstance - Level: %d, XP: %d"), 
			  TeamLevel, TeamXP);

		if (TeamLevel > 1)
		{
			FTimerHandle TempTimer;
			GetWorld()->GetTimerManager().SetTimer(
				TempTimer,
				this,
				&AOmochaGameStateBase::UpdateAllPlayerLevel,
				1.0f,  // 1초 후
				false
			);
		}

		BroadcastXPChange();
	}
}

void AOmochaGameStateBase::CheckLevelUp()
{
	int32 OldLevel = TeamLevel;

	while (TeamLevel < XPRequirements.Num() + 1)
	{
		int32 RequiredXP = GetXPRequiredForLevel(TeamLevel + 1);

		if (RequiredXP > 0 && TeamXP >= RequiredXP)
		{
			TeamLevel++;
			UE_LOG(LogTemp, Warning, TEXT("GameState: TEAM LEVEL UP! %d -> %d"), OldLevel, TeamLevel);
		}

		else
		{
			break;
		}
	}

	if (TeamLevel > OldLevel)
	{
		OnTeamLevelChanged.Broadcast(TeamLevel, OldLevel);

		UpdateAllPlayerLevel();
	}
}

void AOmochaGameStateBase::UpdateAllPlayerLevel()
{
	if (!HasAuthority()) return;

	for(APlayerState* PS : PlayerArray)
	{
		if (AOmochaPlayerState* OmochaPS = Cast<AOmochaPlayerState>(PS))
		{
			if (UAbilitySystemComponent* ASC = OmochaPS->GetAbilitySystemComponent())
			{
				UpdatePlayersLevel(ASC, TeamLevel);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Updated all players' Level to %d"), TeamLevel);
	
}

void AOmochaGameStateBase::UpdatePlayersLevel(UAbilitySystemComponent* ASC, int32 NewLevel)
{
	if (!ASC) return;
    
	if (const UOmochaAttributeSet* ConstAttributeSet = Cast<UOmochaAttributeSet>(ASC->GetAttributeSet(UOmochaAttributeSet::StaticClass())))
	{
		UOmochaAttributeSet* AttributeSet = const_cast<UOmochaAttributeSet*>(ConstAttributeSet);
		AttributeSet->SetLevel(static_cast<float>(NewLevel));
        
		UE_LOG(LogTemp, Warning, TEXT("Level directly updated to: %.1f"), AttributeSet->GetLevel());
	}
}

void AOmochaGameStateBase::BackupToGameInstance()
{
	if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		GI->UpdateMainTainData(TeamLevel, TeamXP);
	}
}

void AOmochaGameStateBase::InitializeXPTable()
{
	if (XPRequirements.Num() == 0)
	{
		// XP Function - Required XP, Level2 = 100, Level3 = 250, Level4 = 450..
		for (int32 Level = 2; Level <= 50; Level++)
		{
			int32 RequiredXP = 50 * Level * Level - 50 * Level;
			XPRequirements.Add(RequiredXP);
		}

		UE_LOG(LogTemp, Log, TEXT("GameState: XP Table initialized with %d levels"), XPRequirements.Num());
	}
}

void AOmochaGameStateBase::BroadcastXPChange()
{
	// UI Update
	const int32 RequiredXP = GetXPRequiredForNextLevel();
	const float XPPercent = GetXPPercent();

	OnTeamXPChanged.Broadcast(TeamXP, RequiredXP, XPPercent);
}

void AOmochaGameStateBase::OnRep_TeamLevel(int32 OldLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("GameState: Team Level replicated: %d -> %d"), OldLevel, TeamLevel);
	OnTeamLevelChanged.Broadcast(TeamLevel, OldLevel);
	BroadcastXPChange();
}

void AOmochaGameStateBase::OnRep_TeamXP(int32 OldXP)
{
	UE_LOG(LogTemp, Log, TEXT("GameState: Team XP replicated: %d -> %d"), OldXP, TeamXP);
	BroadcastXPChange();
}

int32 AOmochaGameStateBase::GetXPRequiredForLevel(int32 Level) const
{
	if (Level <= 1) return 0;

	int32 Index = Level - 2;

	if (Index >= 0 && Index < XPRequirements.Num())
	{
		return XPRequirements[Index];
	}

	return 50 * Level * Level - 50 * Level;
}

int32 AOmochaGameStateBase::GetXPRequiredForNextLevel() const
{
	return GetXPRequiredForLevel(TeamLevel + 1);
}

float AOmochaGameStateBase::GetXPPercent() const
{
	if (TeamLevel <= 1)
	{
		int32 NextLevelXP = GetXPRequiredForNextLevel();
		return NextLevelXP > 0 ? (float)TeamXP / (float)NextLevelXP : 0.0f;
	}

	int32 CurrentLevelXP = GetXPRequiredForLevel(TeamLevel);
	int32 NextLevelXP = GetXPRequiredForNextLevel();

	if (NextLevelXP > CurrentLevelXP)
	{
		int32 XPInCurrentLevel = TeamXP - CurrentLevelXP;
		int32 XPNeededForNextLevel = NextLevelXP - CurrentLevelXP;
		return FMath::Clamp((float)XPInCurrentLevel / (float)XPNeededForNextLevel, 0.0f, 1.0f);
	}

	return 1.0f;
}