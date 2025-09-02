// Copyright It's My Turn


#include "Game/OmochaGameStateBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

void AOmochaGameStateBase::AddTeamXP(float XPAmount)
{
	if (!HasAuthority() || XPAmount <= 0)
	{
		return;
	}

	TeamXP += XPAmount;

	CheckLevelUp();

	UpdateAllPlayerXPAndLevel();

	BroadcastXPChange();
}

void AOmochaGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	BackupToGameInstance();
	Super::EndPlay(EndPlayReason);
}

void AOmochaGameStateBase::InitializeFromGameInstance()
{
	if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		int32 MainTainLevel;
		float MainTainXP;

		if (GI == nullptr)
		{
			return;
		}

		GI->GetMainTainLevelData(MainTainLevel, MainTainXP);

		TeamLevel = MainTainLevel;
		TeamXP = MainTainXP;

		if (TeamLevel > 1)
		{
			FTimerHandle TempTimer;
			GetWorld()->GetTimerManager().SetTimer(
				TempTimer,
				this,
				&AOmochaGameStateBase::UpdateAllPlayerXPAndLevel,
				1.0f, 
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
		}

		else
		{
			break;
		}
	}

	if (TeamLevel > OldLevel)
	{
		OnTeamLevelChanged.Broadcast(TeamLevel, OldLevel);

		UpdateAllPlayerXPAndLevel();

		for (APlayerState* PS : PlayerArray)
		{
			if (AOmochaPlayerState* OmochaPS = Cast<AOmochaPlayerState>(PS))
			{
				OmochaPS->TriggerLevelUpChoice();
			}
		}
	}
}

void AOmochaGameStateBase::UpdateAllPlayerXPAndLevel()
{
	if (!HasAuthority())
	{
		return;
	}

	BackupToGameInstance();

	for (APlayerState* PS : PlayerArray)
	{
		if (AOmochaPlayerState* OmochaPS = Cast<AOmochaPlayerState>(PS))
		{
			if (UAbilitySystemComponent* ASC = OmochaPS->GetAbilitySystemComponent())
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SetLevelGEClass, 1.0f, Context);

				FGameplayTag LevelTag = FGameplayTag::RequestGameplayTag(FName("Attributes.Current.Level"));
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
					SpecHandle,
					LevelTag,
					TeamLevel
				);

				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}

	
}

void AOmochaGameStateBase::UpdatePlayersLevel(UAbilitySystemComponent* ASC, int32 NewLevel)
{
	if (!ASC)
	{
		return;
	}

	if (const UOmochaAttributeSet* ConstAttributeSet = Cast<UOmochaAttributeSet>(
		ASC->GetAttributeSet(UOmochaAttributeSet::StaticClass())))
	{
		UOmochaAttributeSet* AttributeSet = const_cast<UOmochaAttributeSet*>(ConstAttributeSet);
		AttributeSet->SetLevel(static_cast<float>(NewLevel));
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
			float RequiredXP = 50 * Level * Level - 50 * Level;
			XPRequirements.Add(RequiredXP);
		}
	}
}

void AOmochaGameStateBase::BroadcastXPChange()
{
	// UI Update
	const float RequiredXP = GetXPRequiredForNextLevel();
	const float XPPercent = GetXPPercent();

	OnTeamXPChanged.Broadcast(TeamXP, RequiredXP, XPPercent);
}

void AOmochaGameStateBase::OnRep_TeamLevel(int32 OldLevel)
{
	OnTeamLevelChanged.Broadcast(TeamLevel, OldLevel);
	BroadcastXPChange();
}

void AOmochaGameStateBase::OnRep_TeamXP(float OldXP)
{
	BroadcastXPChange();
}

int32 AOmochaGameStateBase::GetXPRequiredForLevel(int32 Level) const
{
	if (Level <= 1)
	{
		return 0;
	}

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
		return NextLevelXP > 0 ? static_cast<float>(TeamXP) / static_cast<float>(NextLevelXP) : 0.0f;
	}

	int32 CurrentLevelXP = GetXPRequiredForLevel(TeamLevel);
	int32 NextLevelXP = GetXPRequiredForNextLevel();

	if (NextLevelXP > CurrentLevelXP)
	{
		int32 XPInCurrentLevel = TeamXP - CurrentLevelXP;
		int32 XPNeededForNextLevel = NextLevelXP - CurrentLevelXP;
		return FMath::Clamp(static_cast<float>(XPInCurrentLevel) / static_cast<float>(XPNeededForNextLevel), 0.0f,
		                    1.0f);
	}

	return 1.0f;
}
