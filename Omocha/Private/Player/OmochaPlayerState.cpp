// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OmochaPlayerState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "Component/OmochaWeaponComponent.h"
#include "Game/OmochaGameInstance.h"
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

void AOmochaPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FTimerHandle RestoreTimer;
		GetWorld()->GetTimerManager().SetTimer(RestoreTimer, [this]()
		{
			RestoreLevelPointFromGameInstance();
		}, 0.5f, false);
        
		UE_LOG(LogTemp, Warning, TEXT("PlayerState BeginPlay - LevelPoint: %d"), LevelPoint);
	}
}

void AOmochaPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOmochaPlayerState, Level);
	DOREPLIFETIME(AOmochaPlayerState, bHasVotedToSkip);
	DOREPLIFETIME(AOmochaPlayerState, bCanVoteToSkip);
	DOREPLIFETIME(AOmochaPlayerState, EquippedWeaponRow);
	DOREPLIFETIME(AOmochaPlayerState, CurrentChoices);
	DOREPLIFETIME(AOmochaPlayerState, LevelPoint);
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
	if (!HasAuthority() || LevelPoint <= 0) 
	{
		return;
	}
	
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
	
	if(HasAuthority())
	{
		OnLevelUpChoiceReady.Broadcast(CurrentChoices);
	}
}

void AOmochaPlayerState::AddPendingLevelUp()
{
	if (!HasAuthority())
	{
		return;
	}
	
	LevelPoint++;

	BackupLevelPointToGameInstance();
	
	if (CurrentChoices.Num() == 0)
	{
		TriggerLevelUpChoice();
	}
}

void AOmochaPlayerState::BackupLevelPointToGameInstance()
{
	if (!HasAuthority()) return;
	if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		const FString OmochaPlayerId = UOmochaGameInstance::GetUniqueId(this);
		GI->UpdatePlayerLevelPoint(OmochaPlayerId, LevelPoint);
	}
}

void AOmochaPlayerState::RestoreLevelPointFromGameInstance()
{
	if (!HasAuthority()) return;
    
	if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		const FString OmochaPlayerId = UOmochaGameInstance::GetUniqueId(this);
		int32 SavedLevelPoint = GI->GetPlayerLevelPoint(OmochaPlayerId);
        
		if (SavedLevelPoint > 0)
		{
			LevelPoint = SavedLevelPoint;
			if (CurrentChoices.Num() == 0)
			{
				TriggerLevelUpChoice();
			}
		}
	}
}

void AOmochaPlayerState::Server_SelectAttributeUpgrade_Implementation(int32 ChoiceIndex)
{
	if (!CurrentChoices.IsValidIndex(ChoiceIndex))
	{
		return;
	}

	FAttributeUpgrade SelectedUpgrade = CurrentChoices[ChoiceIndex];

	ApplyAttributeUpgrade(SelectedUpgrade);

	CurrentChoices.Empty();
	LevelPoint--;

	BackupLevelPointToGameInstance();
	
	if (LevelPoint > 0)
	{
		TriggerLevelUpChoice();
	}
}

void AOmochaPlayerState::OnRep_CurrentChoices()
{
	if (!HasAuthority() && CurrentChoices.Num() > 0)
	{
		OnLevelUpChoiceReady.Broadcast(CurrentChoices);
	}
}

void AOmochaPlayerState::ApplyAttributeUpgrade(const FAttributeUpgrade& Upgrade)
{
	UE_LOG(LogTemp, Warning, TEXT("=== Applying Upgrade ==="));
	UE_LOG(LogTemp, Warning, TEXT("AttributeTag: %s"), *Upgrade.AttributeTag.ToString());
	UE_LOG(LogTemp, Warning, TEXT("UpgradeAmount: %f"), Upgrade.UpgradeAmount);
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		const UOmochaAttributeSet* AttrSet = Cast<UOmochaAttributeSet>(AttributeSet);
		if (AttrSet)
		{
			const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
			
			if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_AttackDamage)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - AttackDamage: %f"), AttrSet->GetAttackDamage());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_MoveSpeed)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - MoveSpeed: %f"), AttrSet->GetMoveSpeed());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_LifeSteal)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - LifeSteal: %f"), AttrSet->GetLifeSteal());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_CriticalChance)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - CriticalChance: %f"), AttrSet->GetCriticalChance());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_CriticalDamage)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - CriticalDamage: %f"), AttrSet->GetCriticalDamage());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_AttackSpeed)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - AttackSpeed: %f"), AttrSet->GetAttackSpeed());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Character_Common_AttackPenetrationCount)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - AttackPenetrationCount: %f"), AttrSet->GetAttackPenetrationCount());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - MovementSkillRangeMultiplier: %f"), AttrSet->GetMovementSkillRangeMultiplier());
			}
			else if (Upgrade.AttributeTag == GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier)
			{
				UE_LOG(LogTemp, Warning, TEXT("BEFORE - AttackProjectileSpeedMultiplier: %f"), AttrSet->GetAttackProjectileSpeedMultiplier());
			}
		}

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
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_CriticalDamage, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_AttackSpeed, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Character_Common_AttackPenetrationCount, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier, 0.0f);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier, 0.0f);
			
			// Select Attribute Upgrade
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				SpecHandle,
				Upgrade.AttributeTag,
				Upgrade.UpgradeAmount
			);

			// Gameplay Effect apply
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Upgrade, AttrSet]()
			{
				if (AttrSet)
				{
					const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
					
					if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_AttackDamage)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - AttackDamage: %f"), AttrSet->GetAttackDamage());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_MoveSpeed)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - MoveSpeed: %f"), AttrSet->GetMoveSpeed());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_LifeSteal)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - LifeSteal: %f"), AttrSet->GetLifeSteal());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_CriticalChance)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - CriticalChance: %f"), AttrSet->GetCriticalChance());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_CriticalDamage)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - CriticalDamage: %f"), AttrSet->GetCriticalDamage());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Current_AttackSpeed)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - AttackSpeed: %f"), AttrSet->GetAttackSpeed());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Character_Common_AttackPenetrationCount)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - AttackPenetrationCount: %f"), AttrSet->GetAttackPenetrationCount());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - MovementSkillRangeMultiplier: %f"), AttrSet->GetMovementSkillRangeMultiplier());
					}
					else if (Upgrade.AttributeTag == GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier)
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTER - AttackProjectileSpeedMultiplier: %f"), AttrSet->GetAttackProjectileSpeedMultiplier());
					}
					
					UE_LOG(LogTemp, Warning, TEXT("=== Upgrade Complete ==="));
				}
			}, 0.1f, false);
		}
	}
}