// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"

float UOmochaGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}

void UOmochaGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (bBlockedWhenDead)
	{
		const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
		if (!ActivationBlockedTags.HasTag(GameplayTags.State_Dead))
		{
			ActivationBlockedTags.AddTag(GameplayTags.State_Dead);
		}
	}
}

void UOmochaGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const FGameplayTag CoolDownTimeTag = FGameplayTag::RequestGameplayTag(FName("Data.CoolDown"));
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE)
	{
		return;
	}
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
	if (!SpecHandle.IsValid())
	{
		return;
	}
	SpecHandle.Data->SetSetByCallerMagnitude(CoolDownTimeTag, CooldownDuration);
	SpecHandle.Data->DynamicGrantedTags.AddTag(AbilityCooldownTag);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
