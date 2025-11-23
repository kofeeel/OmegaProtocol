// Copyright It's My Turn


#include "AbilitySystem/Abilities/BlockDashAbility.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "AsyncTask/WaitCooldownChange.h"
#include "OmochaGameplayTags.h"
#include "Player/OmochaPlayerState.h"
#include "GameFramework/Pawn.h"

bool UBlockDashAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!AvatarPawn)
	{
		return false;
	}

	const AOmochaPlayerState* PS = AvatarPawn->GetPlayerState<AOmochaPlayerState>();
	if (!PS) return false;

	const UOmochaAttributeSet* Attributes = Cast<UOmochaAttributeSet>(PS->GetAttributeSet());
	if (!Attributes)
	{
		return false;
	}

	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE && ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(AbilityCooldownTag))
	{
		return false;
	}

	return Attributes->GetPoundCharges() > 0;
}

bool UBlockDashAbility::CommitAndConsumeCharge()
{
	const FGameplayAbilitySpecHandle Handle = GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

	if (!CheckCooldown(Handle, ActorInfo))
	{
		return false;
	}

	const FActiveGameplayEffectHandle ConsumeHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, ConsumeChargeEffect->GetDefaultObject<UGameplayEffect>(), 1.f);
	const FActiveGameplayEffectHandle RegenCooldownHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, ChargeRegenCooldownEffect->GetDefaultObject<UGameplayEffect>(), 1.f);

	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	UWaitCooldownChange* WaitCooldownTask = UWaitCooldownChange::WaitCooldownChange(GetAbilitySystemComponentFromActorInfo(), GameplayTags.Cooldown_Pound_Charge);
	if (WaitCooldownTask)
	{
		WaitCooldownTask->CooldownComplete.AddDynamic(this, &UBlockDashAbility::OnChargeCooldownEnd);
	}

	const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	const AOmochaPlayerState* PS = AvatarPawn ? AvatarPawn->GetPlayerState<AOmochaPlayerState>() : nullptr;
	UOmochaAttributeSet* Attributes = PS ? Cast<UOmochaAttributeSet>(PS->GetAttributeSet()) : nullptr;
	
	if (Attributes && Attributes->GetPoundCharges() <= 0.f)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
	
	return true;
}


void UBlockDashAbility::OnChargeCooldownEnd(float TimeRemaining)
{
	if(GetCurrentActorInfo())
	{
		const FActiveGameplayEffectHandle AddChargeHandle = ApplyGameplayEffectToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), AddChargeEffect->GetDefaultObject<UGameplayEffect>(), 1.f);
	}
}
