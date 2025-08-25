// Copyright It's My Turn

#include "AbilitySystem/Abilities/Skills/BowLightningChargeAbility.h"
#include "Omocha/Public/AbilitySystem/OmochaAbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAttributeSet.h"

UBowLightningChargeAbility::UBowLightningChargeAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ChargeLevelData.SetNum(4); 
}

void UBowLightningChargeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bAbilityEnded = false;

	ApplyChargeEffect(0);
	
	GetWorld()->GetTimerManager().SetTimer(ChargeLevelTimer,this,
		&UBowLightningChargeAbility::UpdateChargeLevel,1.f, true);
	// Auto Fire in 3sec 
	GetWorld()->GetTimerManager().SetTimer(AutoFireTimer,this,
		&UBowLightningChargeAbility::FireProjectile,MaxChargeTime, false);
}

void UBowLightningChargeAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bAbilityEnded) { FireProjectile(); }
}

void UBowLightningChargeAbility::UpdateChargeLevel()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	int32 CurrLevel = FMath::RoundToInt(ASC->GetNumericAttribute(UOmochaAttributeSet::GetChargeLevelAttribute()));

	if (CurrLevel < 3)
	{
		ApplyChargeEffect(CurrLevel + 1);
	}
}

void UBowLightningChargeAbility::FireProjectile()
{
	if (bAbilityEnded) return;

	bAbilityEnded = true;

	GetWorld()->GetTimerManager().ClearTimer(ChargeLevelTimer);
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
	
	FGameplayEventData EventData;
	const FGameplayTag NextEventTag = FOmochaGameplayTags::Get().Event_AbilitySequence_Next;
	GetAbilitySystemComponentFromActorInfo()->HandleGameplayEvent(NextEventTag, &EventData);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBowLightningChargeAbility::ApplyChargeEffect(int32 Level)
{
	if (!ChargeEffect) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	if (ChargeEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ChargeEffectHandle);
	}

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ChargeEffect, 1, ContextHandle);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FOmochaGameplayTags::Get().Attributes_Current_ChargeLevel, Level);
		ChargeEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void UBowLightningChargeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ChargeLevelTimer);
		GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
	}
	
	if (ChargeEffectHandle.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ChargeEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}