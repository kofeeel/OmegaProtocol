// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "OmochaGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/OmochaCombatInterface.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "AbilitySystem/OmochaBuildSystemLibrary.h"
#include "Character/OmochaCharacterBase.h"
#include "Character/OmochaEnemy.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "DataAsset/SkillBuildData.h"
#include "Game/OmochaGameStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Player/OmochaPlayerController.h"

UOmochaAttributeSet::UOmochaAttributeSet()
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

	/* Current Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Omega, GetOmegaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_AttackDamage, GetAttackDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_SkillDamage, GetSkillDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_Shield, GetShieldAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_MoveSpeed, GetMoveSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_AttackSpeed, GetAttackSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_CoolDownReduction, GetCoolDownReductionAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_AttackRange, GetAttackRangeAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_Level, GetLevelAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Current_CriticalChance, GetCriticalChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_CriticalDamage, GetCriticalDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Current_DodgeChance, GetDodgeChanceAttribute);

	// TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Electric, GetLightningResistanceAttribute);
	// TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	// TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Rust, GetPhysicalResistanceAttribute);

	/* Max Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxOmega, GetMaxOmegaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Max_MaxMoveSpeed, GetMaxMoveSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Max_MaxAttackSpeed, GetMaxAttackSpeedAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Max_MaxCoolDownReduction, GetMaxCoolDownReductionAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Max_MaxLevel, GetMaxLevelAttribute);

	// Character Attributes
	// Zenith
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Zenith_ChainCount, GetChainCountAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Zenith_ChargingTimeMultiplier,
	                     GetChargingTimeMultiplierAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Zenith_CanRotateWhileCharging,
	                     GetCanRotateWhileChargingAttribute);

	// Vulcan
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Vulcan_HasBuffRadius, GetHasBuffRadiusAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Vulcan_BuffRadiusMultiplier,
	                     GetBuffRadiusMultiplierAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Vulcan_UltimateBezierCount, GetUltimateBezierCountAttribute);

	// Rusty
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Rusty_HookSizeMultiplier, GetHookSizeMultiplierAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Rusty_HookSpeedMultiplier, GetHookSpeedMultiplierAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Rusty_UltimateGravityPull, GetUltimateGravityPullAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Rusty_ShieldMaxHealth, GetShieldMaxHealthAttribute);

	// Common
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Common_AttackPenetrationCount,
	                     GetAttackPenetrationCountAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier,
	                     GetMovementSkillRangeMultiplierAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier,
	                     GetAttackProjectileSpeedMultiplierAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Character_Rusty_PoundCharges, GetPoundChargesAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Character_Rusty_MaxPoundCharges, GetMaxPoundChargesAttribute);
}

void UOmochaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Current Attributes 
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, Omega, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, SkillDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, CoolDownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, AttackRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, ChargeLevel, COND_None, REPNOTIFY_Always);

	// Critical/Dodge/LifeSteal Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, CriticalDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, DodgeChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, LifeSteal, COND_None, REPNOTIFY_Always);

	// Max Attributes 
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxOmega, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxMoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxAttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxCoolDownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);

	// Character Attributes
	// Zenith
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, ChainCount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, ChargingTimeMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, CanRotateWhileCharging, COND_None, REPNOTIFY_Always);

	// Vulcan
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, HasBuffRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, BuffRadiusMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, UltimateBezierCount, COND_None, REPNOTIFY_Always);

	// Rusty
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, HookSizeMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, HookSpeedMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, UltimateGravityPull, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, ShieldMaxHealth, COND_None, REPNOTIFY_Always);

	// Common
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, AttackPenetrationCount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MovementSkillRangeMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, AttackProjectileSpeedMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, PoundCharges, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOmochaAttributeSet, MaxPoundCharges, COND_None, REPNOTIFY_Always);
}

void UOmochaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMoveSpeed());
	}
	if (Attribute == GetAttackSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAttackSpeed());
	}
	if (Attribute == GetCoolDownReductionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxCoolDownReduction());
	}
	if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, GetMaxLevel());
	}
	if (Attribute == GetPoundChargesAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPoundCharges());
	}
	if (Attribute == GetOmegaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxOmega());
	}
}

void UOmochaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	/*if (Data.EvaluatedData.Attribute == GetOmegaAttribute())
	{
		HandleOmegaGauge(Props);
	}*/

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}

	if (Data.EvaluatedData.Attribute == GetIncomingXpAttribute())
	{
		HandleIncomingExp(Props);
	}

	// PreAttributeChange First Setting? 
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (GetHealth() <= 0.f && Props.TargetCharacter)
		{
			if (IOmochaCombatInterface* CombatInterface = Cast<IOmochaCombatInterface>(Props.TargetAvatarActor))
			{
				CombatInterface->Die();
			}
		}
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		SetMoveSpeed(FMath::Clamp(GetMoveSpeed(), 0.f, GetMaxMoveSpeed()));

		if (ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
		{
			if (UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(
				Character->GetMovementComponent()))
			{
				Movement->MaxWalkSpeed = GetMoveSpeed();
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetAttackSpeedAttribute())
	{
		SetAttackSpeed(FMath::Clamp(GetAttackSpeed(), 0.f, GetMaxAttackSpeed()));
	}
	if (Data.EvaluatedData.Attribute == GetCoolDownReductionAttribute())
	{
		SetCoolDownReduction(FMath::Clamp(GetCoolDownReduction(), 0.f, GetMaxCoolDownReduction()));
	}
	if (Data.EvaluatedData.Attribute == GetLevelAttribute())
	{
		SetLevel(FMath::Clamp(GetLevel(), 1.f, GetMaxLevel()));
	}
	
	if (Data.EvaluatedData.Attribute == GetPoundChargesAttribute())
	{
		SetPoundCharges(FMath::Clamp(GetPoundCharges(), 0.f, GetMaxPoundCharges()));
	}
}

void UOmochaAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && GetHealth() > NewValue)
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		const float CurrentHealthPercentage = (OldValue > 0.f) ? (GetHealth() / OldValue) : 1.f;
		const float NewHealthValue = NewValue * CurrentHealthPercentage;
		ASC->ApplyModToAttributeUnsafe(GetHealthAttribute(), EGameplayModOp::Override, NewHealthValue);
	}
	if (APlayerState* PS = Cast<APlayerState>(GetOwningActor()))
	{
		if (ACharacter* TargetCharacter = PS->GetPawn<ACharacter>())
		{
			if (Attribute == GetMoveSpeedAttribute())
			{
				if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
				{
					MovementComp->MaxWalkSpeed = NewValue;
				}
			}
		}
	}
}

#pragma region OnRep

void UOmochaAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, Health, OldHealth);
}

void UOmochaAttributeSet::OnRep_Omega(const FGameplayAttributeData& OldOmega) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, Omega, OldOmega);
}

void UOmochaAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, Resilience, OldResilience);
}

void UOmochaAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, AttackDamage, OldAttackDamage);
}

void UOmochaAttributeSet::OnRep_SkillDamage(const FGameplayAttributeData& OldSkillDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, SkillDamage, OldSkillDamage);
}

void UOmochaAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, Shield, OldShield);
}

void UOmochaAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MoveSpeed, OldMoveSpeed);
	if (APlayerState* PS = Cast<APlayerState>(GetOwningActor()))
	{
		if (ACharacter* TargetCharacter = PS->GetPawn<ACharacter>())
		{
			if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
			{
				MovementComp->MaxWalkSpeed = GetMoveSpeed();
			}
		}
	}
}

void UOmochaAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, AttackSpeed, OldAttackSpeed);
}

void UOmochaAttributeSet::OnRep_CoolDownReduction(const FGameplayAttributeData& OldCoolDownReduction) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, CoolDownReduction, OldCoolDownReduction);
}

void UOmochaAttributeSet::OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, AttackRange, OldAttackRange);
}

void UOmochaAttributeSet::OnRep_KnockbackResistance(const FGameplayAttributeData& OldKnockbackResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, KnockbackResistance, OldKnockbackResistance);
}

void UOmochaAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, Level, OldLevel);
}

void UOmochaAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, CriticalChance, OldCriticalChance);
}

void UOmochaAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, CriticalDamage, OldCriticalDamage);
}

void UOmochaAttributeSet::OnRep_DodgeChance(const FGameplayAttributeData& OldDodgeChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, DodgeChance, OldDodgeChance);
}

void UOmochaAttributeSet::OnRep_LifeSteal(const FGameplayAttributeData& OldLifeSteal) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, LifeSteal, OldLifeSteal);
}

void UOmochaAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxHealth, OldMaxHealth);
}

void UOmochaAttributeSet::OnRep_MaxOmega(const FGameplayAttributeData& OldMaxOmega) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxOmega, OldMaxOmega);
}

void UOmochaAttributeSet::OnRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxMoveSpeed, OldMaxMoveSpeed);
}

void UOmochaAttributeSet::OnRep_MaxAttackSpeed(const FGameplayAttributeData& OldMaxAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxAttackSpeed, OldMaxAttackSpeed);
}

void UOmochaAttributeSet::OnRep_MaxCoolDownReduction(const FGameplayAttributeData& OldMaxCoolDownReduction) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxCoolDownReduction, OldMaxCoolDownReduction);
}

void UOmochaAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldMaxLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxLevel, OldMaxLevel);
}

void UOmochaAttributeSet::OnRep_ChargeLevel(const FGameplayAttributeData& OldChargeLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, ChargeLevel, OldChargeLevel);
}

// Character Attributes
// Zenith
void UOmochaAttributeSet::OnRep_ChainCount(const FGameplayAttributeData& OldChainCount) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, ChainCount, OldChainCount);
}

void UOmochaAttributeSet::OnRep_ChargingTimeMultiplier(const FGameplayAttributeData& OldChargingTimeMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, ChargingTimeMultiplier, OldChargingTimeMultiplier);
}

void UOmochaAttributeSet::OnRep_CanRotateWhileCharging(const FGameplayAttributeData& OldCanRotateWhileCharging) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, CanRotateWhileCharging, OldCanRotateWhileCharging);
}

// Vulcan
void UOmochaAttributeSet::OnRep_HasBuffRadius(const FGameplayAttributeData& OldHasBuffRadius) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, HasBuffRadius, OldHasBuffRadius);
}

void UOmochaAttributeSet::OnRep_BuffRadiusMultiplier(const FGameplayAttributeData& OldBuffRadiusMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, BuffRadiusMultiplier, OldBuffRadiusMultiplier);
}

void UOmochaAttributeSet::OnRep_UltimateBezierCount(const FGameplayAttributeData& OldUltimateBezierCount) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, UltimateBezierCount, OldUltimateBezierCount);
}

// Rusty
void UOmochaAttributeSet::OnRep_HookSizeMultiplier(const FGameplayAttributeData& OldHookSizeMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, HookSizeMultiplier, OldHookSizeMultiplier);
}

void UOmochaAttributeSet::OnRep_HookSpeedMultiplier(const FGameplayAttributeData& OldHookSpeedMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, HookSpeedMultiplier, OldHookSpeedMultiplier);
}

void UOmochaAttributeSet::OnRep_UltimateGravityPull(const FGameplayAttributeData& OldUltimateGravityPull) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, UltimateGravityPull, OldUltimateGravityPull);
}

void UOmochaAttributeSet::OnRep_ShieldMaxHealth(const FGameplayAttributeData& OldShieldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, ShieldMaxHealth, OldShieldMaxHealth);
}

// Common
void UOmochaAttributeSet::OnRep_AttackPenetrationCount(const FGameplayAttributeData& OldAttackPenetrationCount) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, AttackPenetrationCount, OldAttackPenetrationCount);
}

void UOmochaAttributeSet::OnRep_MovementSkillRangeMultiplier(
	const FGameplayAttributeData& OldMovementSkillRangeMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MovementSkillRangeMultiplier, OldMovementSkillRangeMultiplier);
}

void UOmochaAttributeSet::OnRep_AttackProjectileSpeedMultiplier(
	const FGameplayAttributeData& OldAttackProjectileSpeedMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, AttackProjectileSpeedMultiplier,
	                            OldAttackProjectileSpeedMultiplier);
}

void UOmochaAttributeSet::OnRep_PoundCharges(const FGameplayAttributeData& OldPoundCharges) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, PoundCharges, OldPoundCharges);
}

void UOmochaAttributeSet::OnRep_MaxPoundCharges(const FGameplayAttributeData& OldMaxPoundCharges) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOmochaAttributeSet, MaxPoundCharges, OldMaxPoundCharges);
}
#pragma endregion

void UOmochaAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
                                              FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC)
		&& Props.SourceASC->AbilityActorInfo.IsValid()
		&& Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid()
		&& Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UOmochaAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit,
                                           bool bCriticalHit) const
{
	if (!IsValid(Props.SourceCharacter) || !IsValid(Props.TargetCharacter))
	{
		return;
	}

	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AOmochaPlayerController* PC = Cast<AOmochaPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}

		if (AOmochaPlayerController* PC = Cast<AOmochaPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

//Todo HandleExp
void UOmochaAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.SourceCharacter && Props.TargetCharacter && Props.SourceCharacter != Props.TargetCharacter)
	{
		if (UAbilitySystemComponent* SourceASC = Props.SourceASC)
		{
			// Temporary XP - Change need
			float XPAmount = 50.0f;

			if (AOmochaEnemy* Enemy = Cast<AOmochaEnemy>(Props.TargetCharacter))
			{
				XPAmount = Enemy->GetXPReward();
			}
			
			if (UWorld* World = Props.SourceCharacter->GetWorld())
			{
				if (AOmochaGameStateBase* GameState = Cast<AOmochaGameStateBase>(World->GetGameState()))
				{
					GameState->AddTeamXP(XPAmount);
					UE_LOG(LogTemp, Warning, TEXT("Enemy killed! Giving %f XP directly"), XPAmount);
				}
			}
		}
	}
}

void UOmochaAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	
	FGameplayCueParameters CueParams;
	CueParams.EffectContext = Props.EffectContextHandle; 
	CueParams.RawMagnitude = LocalIncomingDamage;       
	CueParams.Location = Props.TargetAvatarActor->GetActorLocation();

	if (Props.TargetASC && Props.TargetASC->GetOwnerRole() == ROLE_Authority)
	{
		Props.TargetASC->ExecuteGameplayCue(FOmochaGameplayTags::Get().GameplayCue_DamageText, CueParams);
	}
	
	if (LocalIncomingDamage > 0.f)
	{
		// Health Calculation
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bBlock = UOmochaAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		const bool bCriticalHit = UOmochaAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		
		//ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);

		const bool bFatal = NewHealth <= 0.f;
		if (bFatal)
		{
			// Death Event
			if (AOmochaEnemy* Enemy = Cast<AOmochaEnemy>(Props.TargetAvatarActor))
			{
				Enemy->SetLastDamageSource(Props.SourceCharacter);
			}
			Die(Props);
			SendXPEvent(Props);
		}
		else
		{
			FGameplayCueParameters CueParameters;
			CueParameters.EffectContext = Props.EffectContextHandle;
			
			// Player Hitreact
			if (AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(Props.TargetCharacter))
			{
				if (UAbilitySystemComponent* TargetASC = PlayerCharacter->GetAbilitySystemComponent())
				{
					CueParameters.RawMagnitude = LocalIncomingDamage / GetMaxHealth() * 100.0f;
					TargetASC->ExecuteGameplayCue(FOmochaGameplayTags::Get().GameplayCue_HitReact, CueParameters);
				}
			}

			if (Props.TargetAvatarActor->Implements<UOmochaCombatInterface>() &&
				!IOmochaCombatInterface::Execute_IsBeingHitReact(Props.TargetCharacter))
			{
				//HitReact
				FGameplayEventData EventData;
				EventData.Target = Props.TargetAvatarActor;
				EventData.ContextHandle = Props.EffectContextHandle;
				const FGameplayTag EventTag = FOmochaGameplayTags::Get().GameplayEvent_HitReact;
				Props.TargetASC->HandleGameplayEvent(EventTag, &EventData);

				//Knockback
				const FVector KnockbackForce =
					UOmochaAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
				if (!KnockbackForce.IsZero())
				{
					if (IOmochaCombatInterface* CombatInterface = Cast<IOmochaCombatInterface>(Props.TargetAvatarActor))
					{
						CombatInterface->Execute_ApplyKnockback(Props.TargetAvatarActor, KnockbackForce);
					}
				}
			}
		}
	}
}

void UOmochaAttributeSet::HandleOmegaGauge(const FEffectProperties& Props)
{
	const float LocalOmega = GetOmega();
	if (LocalOmega > GetMaxOmega())
	{
		SetOmega(0.f);
		SetHealth(0.f);
		if (Props.TargetCharacter)
		{
			if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(Props.TargetCharacter->GetGameInstance()))
			{
				GI->SetOmega(0.f);
			}
		}
		Die(Props);
	}
}

void UOmochaAttributeSet::Die(const FEffectProperties& Props)
{
	if (const FOmochaGameplayEffectContext* OmochaContext = static_cast<const FOmochaGameplayEffectContext*>(Props.
		EffectContextHandle.Get()))
	{
		FGameplayTag KillingAbility = OmochaContext->GetKillingAbilityTag();
		if (KillingAbility.IsValid())
		{
			UOmochaBuildSystemLibrary::ApplyBuildsForTrigger(Props.SourceASC, Props.TargetASC,
			                                                 EBuildTriggerCondition::OnKill, KillingAbility);
		}
	}

	if (AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(Props.TargetCharacter))
	{
		if (UAbilitySystemComponent* TargetASC = PlayerCharacter->GetAbilitySystemComponent())
		{
			FGameplayCueParameters CueParameters;

			CueParameters.EffectContext = Props.EffectContextHandle;
			CueParameters.RawMagnitude = 100.0f;

			TargetASC->ExecuteGameplayCue(
				FOmochaGameplayTags::Get().GameplayCue_HitReact,
				CueParameters);
		}
	}

	if (IOmochaCombatInterface* CombatInterface = Cast<IOmochaCombatInterface>(Props.TargetAvatarActor))
	{
		CombatInterface->Die();
	}
}

void UOmochaAttributeSet::HandleIncomingExp(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXp();
	SetIncomingXp(0.f);

	if (LocalIncomingXP > 0.f)
	{
		if (UWorld* World = Props.TargetAvatarActor->GetWorld())
		{
			if (AOmochaGameStateBase* GameState = Cast<AOmochaGameStateBase>(World->GetGameState()))
			{
				GameState->AddTeamXP(LocalIncomingXP);
			}
		}
	}
}
