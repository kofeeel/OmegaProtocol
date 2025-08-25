// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OmochaPlayerState.h"

#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"
#include "Game/OmochaGameStateBase.h"
#include "Net/UnrealNetwork.h"

AOmochaPlayerState::AOmochaPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOmochaAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	AttributeSet = CreateDefaultSubobject<UOmochaAttributeSet>("AttributeSet");
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
