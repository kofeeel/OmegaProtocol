// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OmochaPlayerState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "Component/OmochaWeaponComponent.h"
#include "Game/OmochaGameStateBase.h"
#include "Net/UnrealNetwork.h"

AOmochaPlayerState::AOmochaPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOmochaAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	AttributeSet = CreateDefaultSubobject<UOmochaAttributeSet>("AttributeSet");
	SkillBuildComponent = CreateDefaultSubobject<UOmochaSkillBuildComponent>("SkillBuildComponent");
	SetNetUpdateFrequency(100.f);
	NetPriority = 3.f;
}

void AOmochaPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOmochaPlayerState, Level);
	DOREPLIFETIME(AOmochaPlayerState, bHasVotedToSkip);
	DOREPLIFETIME(AOmochaPlayerState, bCanVoteToSkip);
	DOREPLIFETIME(AOmochaPlayerState, EquippedWeaponRow);
	DOREPLIFETIME(AOmochaPlayerState, CurrentChoices);
}

void AOmochaPlayerState::Server_VoteToSkipCinematic_Implementation()
{
	if (!bHasVotedToSkip && bCanVoteToSkip)
	{
		bHasVotedToSkip = true;

		if (AOmochaGameStateBase* OmochaGameState = GetWorld()->GetGameState<AOmochaGameStateBase>())
		{
			OmochaGameState->AddCinematicSkipVote();
		}
	}
}

void AOmochaPlayerState::Server_CancelVoteToSkip_Implementation()
{
	if (bHasVotedToSkip)
	{
		bHasVotedToSkip = false;

		if (AOmochaGameStateBase* OmochaGameState = GetWorld()->GetGameState<AOmochaGameStateBase>())
		{
			OmochaGameState->RemoveCinematicSkipVote();
		}
	}
}

void AOmochaPlayerState::OnRep_HasVotedToSkip()
{
}

void AOmochaPlayerState::SetCanVoteToSkip(bool bCanVote)
{
	if (HasAuthority())
	{
		bCanVoteToSkip = bCanVote;
	}
}

void AOmochaPlayerState::OnRep_EquippedWeapon()
{
	AOmochaPlayerCharacter* PlayerCharacter = GetPawn<AOmochaPlayerCharacter>();
	if (PlayerCharacter && PlayerCharacter->WeaponComponent)
	{
		PlayerCharacter->WeaponComponent.Get()->UpdateWeaponMeshFromState();
	}
}

void AOmochaPlayerState::CopyProperties(APlayerState* PS)
{
	Super::CopyProperties(PS);
}

void AOmochaPlayerState::TriggerLevelUpChoice()
{
	if (!HasAuthority() || LevelPoint <= 0) return;

	CurrentChoices.Empty();
	TArray<int32> UsedIndices;

	if (AvailableUpgrades.Num() == 0)
	{
		return;	
	}

	int32 MaxChoices = FMath::Min(3, AvailableUpgrades.Num());

	while (CurrentChoices.Num() < MaxChoices && UsedIndices.Num() < AvailableUpgrades.Num())
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableUpgrades.Num() - 1);

		if (!UsedIndices.Contains(RandomIndex))
		{
			CurrentChoices.Add(AvailableUpgrades[RandomIndex]);
			UsedIndices.Add(RandomIndex);
		}
	}

	OnRep_CurrentChoices();
}

void AOmochaPlayerState::AddPendingLevelUp()
{
	LevelPoint++;

	if (CurrentChoices.Num() == 0)
	{
		TriggerLevelUpChoice();
	}
}

void AOmochaPlayerState::Server_SelectAttributeUpgrade_Implementation(int32 ChoiceIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Player %s selected upgrade index: %d"), *GetPlayerName(), ChoiceIndex);

	if (!CurrentChoices.IsValidIndex(ChoiceIndex))
	{
		return;
	}

	FAttributeUpgrade SelectedUpgrade = CurrentChoices[ChoiceIndex];

	ApplyAttributeUpgrade(SelectedUpgrade);

	CurrentChoices.Empty();

	LevelPoint--;

	if (LevelPoint > 0)
	{
		TriggerLevelUpChoice();
	}
}

void AOmochaPlayerState::OnRep_CurrentChoices()
{
	if (CurrentChoices.Num() > 0)
	{
		OnLevelUpChoiceReady.Broadcast(CurrentChoices);
	}
}

void AOmochaPlayerState::ApplyAttributeUpgrade(const FAttributeUpgrade& Upgrade)
{
	UE_LOG(LogTemp, Warning, TEXT("=== Attribute Upgrade Debug ==="));
	UE_LOG(LogTemp, Warning, TEXT("AttributeTag: %s"), *Upgrade.AttributeTag.ToString());
	UE_LOG(LogTemp, Warning, TEXT("UpgradeAmount: %f"), Upgrade.UpgradeAmount);
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();

		if (SetAttributeUpgradeGEClass)
		{
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SetAttributeUpgradeGEClass, 1.0f, EffectContext);

			const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_MaxHealth, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_MoveSpeed, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_LifeSteal, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_AttackDamage, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_CriticalChance, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxAttackSpeed, 0.0f);
			
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				SpecHandle,
				Upgrade.AttributeTag,
				Upgrade.UpgradeAmount
			);

			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
