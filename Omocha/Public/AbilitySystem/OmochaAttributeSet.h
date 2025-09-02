// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "OmochaAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties()
	{
	}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

template <class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */
class AOmochaEXPActor;

UCLASS()
class OMOCHA_API UOmochaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UOmochaAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;


#pragma region AttributeData
	//현재 속성
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Current Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Omega, Category = "Current Attributes")
	FGameplayAttributeData Omega;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, Omega);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxOmega, Category = "Current Attributes")
	FGameplayAttributeData MaxOmega;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MaxOmega);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Current Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, Resilience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackDamage, Category = "Current Attributes")
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, AttackDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SkillDamage, Category = "Current Attributes")
	FGameplayAttributeData SkillDamage;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, SkillDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Current Attributes")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Current Attributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Current Attributes")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, AttackSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CoolDownReduction, Category = "Current Attributes")
	FGameplayAttributeData CoolDownReduction;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, CoolDownReduction);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackRange, Category = "Current Attributes")
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, AttackRange);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_KnockbackResistance, Category = "Current Attributes")
	FGameplayAttributeData KnockbackResistance;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, KnockbackResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Current Attributes")
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, Level);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance, Category = "Current Attributes")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, CriticalChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalDamage, Category = "Current Attributes")
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, CriticalDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DodgeChance, Category = "Current Attributes")
	FGameplayAttributeData DodgeChance;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, DodgeChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LifeSteal, Category = "Current Attributes")
	FGameplayAttributeData LifeSteal;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, LifeSteal);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChargeLevel, Category = "Meta Attributes")
	FGameplayAttributeData ChargeLevel;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, ChargeLevel);

	UPROPERTY()
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, IncomingDamage);

	UPROPERTY()
	FGameplayAttributeData IncomingXp;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, IncomingXp);

	//고정 속성 (MAX)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Max Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMoveSpeed, Category = "Max Attributes")
	FGameplayAttributeData MaxMoveSpeed;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MaxMoveSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAttackSpeed, Category = "Max Attributes")
	FGameplayAttributeData MaxAttackSpeed;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MaxAttackSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxCoolDownReduction, Category = "Max Attributes")
	FGameplayAttributeData MaxCoolDownReduction;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MaxCoolDownReduction);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxLevel, Category = "Max Attributes")
	FGameplayAttributeData MaxLevel;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MaxLevel);

	// Character Attributes
	// Zenith
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChainCount, Category = "Character Attributes|Zenith")
	FGameplayAttributeData ChainCount;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, ChainCount);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ChargingTimeMultiplier,
		Category = "Character Attributes|Zenith")
	FGameplayAttributeData ChargingTimeMultiplier;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, ChargingTimeMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CanRotateWhileCharging,
		Category = "Character Attributes|Zenith")
	FGameplayAttributeData CanRotateWhileCharging;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, CanRotateWhileCharging);

	// Vulcan
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HasBuffRadius, Category = "Character Attributes|Vulcan")
	FGameplayAttributeData HasBuffRadius;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, HasBuffRadius);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BuffRadiusMultiplier, Category = "Character Attributes|Vulcan")
	FGameplayAttributeData BuffRadiusMultiplier;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, BuffRadiusMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateBezierCount, Category = "Character Attributes|Vulcan")
	FGameplayAttributeData UltimateBezierCount;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, UltimateBezierCount);

	// Rusty
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HookSizeMultiplier, Category = "Character Attributes|Rusty")
	FGameplayAttributeData HookSizeMultiplier;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, HookSizeMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HookSpeedMultiplier, Category = "Character Attributes|Rusty")
	FGameplayAttributeData HookSpeedMultiplier;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, HookSpeedMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateGravityPull, Category = "Character Attributes|Rusty")
	FGameplayAttributeData UltimateGravityPull;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, UltimateGravityPull);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldMaxHealth, Category = "Character Attributes|Rusty")
	FGameplayAttributeData ShieldMaxHealth;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, ShieldMaxHealth);

	// Common
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPenetrationCount,
		Category = "Character Attributes|Common")
	FGameplayAttributeData AttackPenetrationCount;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, AttackPenetrationCount);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSkillRangeMultiplier,
		Category = "Character Attributes|Common")
	FGameplayAttributeData MovementSkillRangeMultiplier;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, MovementSkillRangeMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackProjectileSpeedMultiplier,
		Category = "Character Attributes|Common")
	FGameplayAttributeData AttackProjectileSpeedMultiplier;
	ATTRIBUTE_ACCESSORS(UOmochaAttributeSet, AttackProjectileSpeedMultiplier);
#pragma endregion

#pragma region OnRep
	// 현재 속성 값 Replication
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_Omega(const FGameplayAttributeData& OldOmega) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage) const;

	UFUNCTION()
	void OnRep_SkillDamage(const FGameplayAttributeData& OldSkillDamage) const;

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield) const;

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const;

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const;

	UFUNCTION()
	void OnRep_CoolDownReduction(const FGameplayAttributeData& OldCoolDownReduction) const;

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange) const;

	UFUNCTION()
	void OnRep_KnockbackResistance(const FGameplayAttributeData& OldKnockbackResistance) const;

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel) const;

	UFUNCTION()
	void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const;

	UFUNCTION()
	void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const;

	UFUNCTION()
	void OnRep_DodgeChance(const FGameplayAttributeData& OldDodgeChance) const;

	UFUNCTION()
	void OnRep_LifeSteal(const FGameplayAttributeData& OldLifeSteal) const;

	// 고정 속성 값 Replication
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_MaxOmega(const FGameplayAttributeData& OldMaxOmega) const;

	UFUNCTION()
	void OnRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed) const;

	UFUNCTION()
	void OnRep_MaxAttackSpeed(const FGameplayAttributeData& OldMaxAttackSpeed) const;

	UFUNCTION()
	void OnRep_MaxCoolDownReduction(const FGameplayAttributeData& OldMaxCoolDownReduction) const;

	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldMaxLevel) const;

	UFUNCTION()
	void OnRep_ChargeLevel(const FGameplayAttributeData& OldChargeLevel) const;

	// Character Attributes
	// Zenith
	UFUNCTION()
	void OnRep_ChainCount(const FGameplayAttributeData& OldChainCount) const;
	UFUNCTION()
	void OnRep_ChargingTimeMultiplier(const FGameplayAttributeData& OldChargingTimeMultiplier) const;
	UFUNCTION()
	void OnRep_CanRotateWhileCharging(const FGameplayAttributeData& OldCanRotateWhileCharging) const;

	// Vulcan
	UFUNCTION()
	void OnRep_HasBuffRadius(const FGameplayAttributeData& OldHasBuffRadius) const;
	UFUNCTION()
	void OnRep_BuffRadiusMultiplier(const FGameplayAttributeData& OldBuffRadiusMultiplier) const;
	UFUNCTION()
	void OnRep_UltimateBezierCount(const FGameplayAttributeData& OldUltimateBezierCount) const;

	// Rusty
	UFUNCTION()
	void OnRep_HookSizeMultiplier(const FGameplayAttributeData& OldHookSizeMultiplier) const;
	UFUNCTION()
	void OnRep_HookSpeedMultiplier(const FGameplayAttributeData& OldHookSpeedMultiplier) const;
	UFUNCTION()
	void OnRep_UltimateGravityPull(const FGameplayAttributeData& OldUltimateGravityPull) const;
	UFUNCTION()
	void OnRep_ShieldMaxHealth(const FGameplayAttributeData& OldShieldMaxHealth) const;

	// Common
	UFUNCTION()
	void OnRep_AttackPenetrationCount(const FGameplayAttributeData& OldAttackPenetrationCount) const;
	UFUNCTION()
	void OnRep_MovementSkillRangeMultiplier(const FGameplayAttributeData& OldMovementSkillRangeMultiplier) const;
	UFUNCTION()
	void OnRep_AttackProjectileSpeedMultiplier(const FGameplayAttributeData& OldAttackProjectileSpeedMultiplier) const;
#pragma endregion

protected:
	virtual void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const;
	void SendXPEvent(const FEffectProperties& Props);
	void Die(const FEffectProperties& Props);
	virtual void HandleIncomingDamage(const FEffectProperties& Props);
	void HandleOmegaGauge(const FEffectProperties& Props);
	void HandleIncomingExp(const FEffectProperties& Props);
};
