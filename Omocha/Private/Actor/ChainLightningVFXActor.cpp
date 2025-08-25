// Copyright It's My Turn


#include "Actor/ChainLightningVFXActor.h"

#include "Net/UnrealNetwork.h"


AChainLightningVFXActor::AChainLightningVFXActor()
{
	bReplicates = true;
	SetLifeSpan(3.0f);
}

void AChainLightningVFXActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AChainLightningVFXActor, TargetChain, COND_InitialOnly);
}

void AChainLightningVFXActor::OnRep_TargetChain()
{
	PlayVFX();
}