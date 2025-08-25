// Copyright It's My Turn

#include "UI/WidgetController/StatWidgetController.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"

void UStatWidgetController::BroadCastInitialValues()
{
	if (!IsValid(AttributeSet))
	{
		return;
	}

	const UOmochaAttributeSet* OmochaAttributeSet = Cast<UOmochaAttributeSet>(AttributeSet);
	if (!OmochaAttributeSet)
	{
		return;
	}

	// Common Stat
	OnAttackDamageChanged.Broadcast(OmochaAttributeSet->GetAttackDamage());
	OnShieldChanged.Broadcast(OmochaAttributeSet->GetShield());
	OnMoveSpeedChanged.Broadcast(OmochaAttributeSet->GetMoveSpeed());
	OnAttackSpeedChanged.Broadcast(OmochaAttributeSet->GetAttackSpeed());
	OnCriticalChanceChanged.Broadcast(OmochaAttributeSet->GetCriticalChance());
	OnCriticalDamageChanged.Broadcast(OmochaAttributeSet->GetCriticalDamage());
	OnLifeStealChanged.Broadcast(OmochaAttributeSet->GetLifeSteal());
	OnSkillSizeMultiplierChanged.Broadcast(OmochaAttributeSet->GetBuffRadiusMultiplier());
	OnSkillRangeMultiplierChanged.Broadcast(OmochaAttributeSet->GetMovementSkillRangeMultiplier());
	OnAttackPenetrationCountChanged.Broadcast(OmochaAttributeSet->GetAttackPenetrationCount());
	OnBulletSpeedMultiplierChanged.Broadcast(OmochaAttributeSet->GetAttackProjectileSpeedMultiplier());

	// Zenith Stat
	OnChainCountChanged.Broadcast(OmochaAttributeSet->GetChainCount());
	OnChargingTimeMultiplierChanged.Broadcast(OmochaAttributeSet->GetChargingTimeMultiplier());

	// Vulcan Stat
	OnUltimateCountChanged.Broadcast(OmochaAttributeSet->GetUltimateBezierCount());
	OnCanBuffRangeChanged.Broadcast(OmochaAttributeSet->GetHasBuffRadius());

	// Rusty Stat
	OnHookSizeMultiplierChanged.Broadcast(OmochaAttributeSet->GetHookSizeMultiplier());
}

void UStatWidgetController::BindCallbacksToDependencies()
{
	if (!IsValid(AbilitySystemComponent) || !IsValid(AttributeSet))
	{
		return;
	}

	const UOmochaAttributeSet* OmochaAttributeSet = Cast<UOmochaAttributeSet>(AttributeSet);
	if (!OmochaAttributeSet)
	{
		return;
	}

	// Common Stat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetAttackDamageAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnAttackDamageChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetShieldAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnShieldChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetMoveSpeedAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnMoveSpeedChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetAttackSpeedAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnAttackSpeedChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetCriticalChanceAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnCriticalChanceChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetCriticalDamageAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnCriticalDamageChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetLifeStealAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnLifeStealChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetBuffRadiusMultiplierAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnSkillSizeMultiplierChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetMovementSkillRangeMultiplierAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnSkillRangeMultiplierChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetAttackPenetrationCountAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnAttackPenetrationCountChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetAttackProjectileSpeedMultiplierAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnBulletSpeedMultiplierChanged.Broadcast(Data.NewValue); });

	// Zenith Stat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetChainCountAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnChainCountChanged.Broadcast(Data.NewValue);
		                        });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetChargingTimeMultiplierAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnChargingTimeMultiplierChanged.Broadcast(Data.NewValue); });

	// Vulcan Stat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetUltimateBezierCountAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnUltimateCountChanged.Broadcast(Data.NewValue); });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetHasBuffRadiusAttribute()).
	                        AddLambda(
		                        [this](const FOnAttributeChangeData& Data)
		                        {
			                        OnCanBuffRangeChanged.Broadcast(Data.NewValue);
		                        });

	// Rusty Stat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetHookSizeMultiplierAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHookSizeMultiplierChanged.Broadcast(Data.NewValue); });
}
