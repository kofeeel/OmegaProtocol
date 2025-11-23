// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "OmochaAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams()
	{
	}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag HitType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;

	UPROPERTY()
	TSoftObjectPtr<UCurveFloat> KnockbackSpeedCurve = nullptr;

	UPROPERTY()
	TSoftObjectPtr<UCurveFloat> KnockbackHeightCurve = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DebuffEffectClass = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DebuffType = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffMagnitude = 0.f;
	
	// Not using
	UPROPERTY(BlueprintReadWrite)
	FVector ImpulseDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	float ImpulseMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag KillingAbilityTag = FGameplayTag();
};

USTRUCT(BlueprintType)
struct FOmochaGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	// Damage Getter
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }
	FGameplayTag GetDamageType() const { return DamageType; }
	FVector GetDeathImpulse() const { return DeathImpulse; }
	bool IsRadialDamage() const { return bIsRadialDamage; }
	float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius; }
	float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius; }
	FVector GetRadialDamageOrigin() const { return RadialDamageOrigin; }
	float GetSkillBaseDamage() const { return SkillBaseDamage; }

	// Debuff Getter
	bool IsSuccessDebuff() const { return bIsSuccessDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	float GetDebuffMagnitude() const { return DebuffMagnitude; }
	float GetDebuffChance() const {return DebuffChance; }
	FGameplayTag GetDebuffType() const { return DebuffType; }
	TSubclassOf<UGameplayEffect> GetDebuffEffectClass() const { return DebuffEffectClass; }
	
	// HitReact Getter
	FGameplayTag GetHitType() const { return HitType; }
	FVector GetImpulseDirection() const { return ImpulseDirection; }
	float GetImpulseMultiplier() const { return ImpulseMultiplier; }
	FVector GetKnockbackForce() const { return KnockbackForce; }
	TSoftObjectPtr<UCurveFloat> GetKnockbackSpeedCurve() const { return KnockbackSpeedCurve; }
	TSoftObjectPtr<UCurveFloat> GetKnockbackHeightCurve() const { return KnockbackHeightCurve; }

	// Killing Ability Getter
	FGameplayTag GetKillingAbilityTag() const { return KillingAbilityTag; }

	// Damage Setter
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetDamageType(FGameplayTag InDamageType) { DamageType = InDamageType; }
	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }
	void SetIsRadialDamage(bool bInIsRadialDamage) { bIsRadialDamage = bInIsRadialDamage; }

	void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius)
	{
		RadialDamageInnerRadius = InRadialDamageInnerRadius;
	}

	void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius)
	{
		RadialDamageOuterRadius = InRadialDamageOuterRadius;
	}

	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) { RadialDamageOrigin = InRadialDamageOrigin; }
	void SetSkillBaseDamage(float InSkillBaseDamage) { SkillBaseDamage = InSkillBaseDamage; }

	// Debuff
	void SetIsSuccessDebuff(bool bInIsSuccessDebuff) { bIsSuccessDebuff = bInIsSuccessDebuff; }
	void SetDebuffDamage(float bInDebuffDamage) { DebuffDamage = bInDebuffDamage; }
	void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDebuffMagnitude(float InMagnitude) { DebuffMagnitude = InMagnitude; }
	void SetDebuffType(const FGameplayTag& InDebuffType) { DebuffType = InDebuffType; }
	void SetDebuffChance(float InDebuffChance){DebuffChance = InDebuffChance;}
	void SetDebuffEffectClass(const TSubclassOf<UGameplayEffect>& InClass) { DebuffEffectClass = InClass; }
	
	// HitReact
	void SetHitType(const FGameplayTag& InHitType) { HitType = InHitType; }
	void SetImpulseDirection(const FVector& InImpulse) { ImpulseDirection = InImpulse; }
	void SetImpulseMultiplier(float InImpulse) { ImpulseMultiplier = InImpulse; }
	void SetKnockbackForce(const FVector& InKnockbackForce) { KnockbackForce = InKnockbackForce; }
	void SetKnockbackSpeedCurve(const TSoftObjectPtr<UCurveFloat>& InSpeedCurve) { KnockbackSpeedCurve = InSpeedCurve; }

	void SetKnockbackHeightCurve(const TSoftObjectPtr<UCurveFloat>& InHeightCurve)
	{
		KnockbackHeightCurve = InHeightCurve;
	}

	// Killing Ability
	void SetKillingAbilityTag(const FGameplayTag& InTag) { KillingAbilityTag = InTag; }


	 virtual UScriptStruct* GetScriptStruct() const override
	 {
	 	return FOmochaGameplayEffectContext::StaticStruct();
	 }
	 
	 virtual FGameplayEffectContext* Duplicate() const override
	 {
	 	FOmochaGameplayEffectContext* NewContext = new FOmochaGameplayEffectContext(*this);
	 	if (GetHitResult())
	 	{
	 		NewContext->AddHitResult(*GetHitResult(), true);
	 	}
	 	return NewContext;
	 }
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	float SkillBaseDamage = 0.f;

	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	FGameplayTag DamageType;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY()
	bool bIsRadialDamage = false;

	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;

	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	UPROPERTY()
	bool bIsSuccessDebuff = false;
	
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DebuffEffectClass;
	
	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DebuffChance = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffMagnitude = 0.f; 
	
	UPROPERTY()
	FGameplayTag DebuffType;
	
	UPROPERTY()
	FVector ImpulseDirection = FVector::ZeroVector;

	UPROPERTY()
	float ImpulseMultiplier = 1.f;

	UPROPERTY()
	TSoftObjectPtr<UCurveFloat> KnockbackSpeedCurve;

	UPROPERTY()
	TSoftObjectPtr<UCurveFloat> KnockbackHeightCurve;

	UPROPERTY()
	FGameplayTag HitType;

	UPROPERTY()
	FGameplayTag KillingAbilityTag;
};

template<>
struct TStructOpsTypeTraits<FOmochaGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FOmochaGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
