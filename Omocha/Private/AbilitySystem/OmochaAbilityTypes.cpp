// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAbilityTypes.h"
#include "GameplayEffect.h"

bool FOmochaGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    // Trace Insight
    TRACE_CPUPROFILER_EVENT_SCOPE(FOmochaGameplayEffectContext::NetSerialize);
    const int32 StartPos = Ar.Tell();
    
    FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);
    
    // Bit Flags
    uint8 RepBits = 0;
    
    if (Ar.IsSaving())
    {
        if (bIsBlockedHit) RepBits |= 1 << 0;
        if (bIsCriticalHit) RepBits |= 1 << 1;
        if (bIsRadialDamage) RepBits |= 1 << 2;
        if (bIsSuccessDebuff) RepBits |= 1 << 3;
    }
    
    Ar << RepBits;
    
    if (Ar.IsLoading())
    {
        bIsBlockedHit = (RepBits & (1 << 0)) != 0;
        bIsCriticalHit = (RepBits & (1 << 1)) != 0;
        bIsRadialDamage = (RepBits & (1 << 2)) != 0;
        bIsSuccessDebuff = (RepBits & (1 << 3)) != 0;
    }
    
    // Damage Data
    Ar << SkillBaseDamage;
    DamageType.NetSerialize(Ar, Map, bOutSuccess);
    Ar << DeathImpulse;
    Ar << KnockbackForce;
    Ar << ImpulseDirection;
    
    // Radial Damage (conditional)
    if (bIsRadialDamage)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*FString::Printf(TEXT("RadialDamage Serialize")));
        Ar << RadialDamageInnerRadius;
        Ar << RadialDamageOuterRadius;
        Ar << RadialDamageOrigin;
    }
    
    // Debuff Data (conditional)
    if (bIsSuccessDebuff)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*FString::Printf(TEXT("Debuff Serialize")));
        Ar << DebuffDamage;
        Ar << DebuffDuration;
        Ar << DebuffChance;
        Ar << DebuffFrequency;
        Ar << DebuffMagnitude;
        DebuffType.NetSerialize(Ar, Map, bOutSuccess);
        
        UObject* DebuffClass = DebuffEffectClass;
        Map->SerializeObject(Ar, UGameplayEffect::StaticClass(), DebuffClass);
        if (Ar.IsLoading())
        {
            DebuffEffectClass = Cast<UClass>(DebuffClass);
        }
    }
    
    // HitReact Data
    Ar << ImpulseMultiplier;
    HitType.NetSerialize(Ar, Map, bOutSuccess);
    
    // Knockback Curves
    FString SpeedCurvePath = KnockbackSpeedCurve.IsNull() ? TEXT("") : KnockbackSpeedCurve.ToString();
    FString HeightCurvePath = KnockbackHeightCurve.IsNull() ? TEXT("") : KnockbackHeightCurve.ToString();
    
    Ar << SpeedCurvePath;
    Ar << HeightCurvePath;
    
    if (Ar.IsLoading())
    {
        KnockbackSpeedCurve = SpeedCurvePath.IsEmpty() ? nullptr : TSoftObjectPtr<UCurveFloat>(FSoftObjectPath(SpeedCurvePath));
        KnockbackHeightCurve = HeightCurvePath.IsEmpty() ? nullptr : TSoftObjectPtr<UCurveFloat>(FSoftObjectPath(HeightCurvePath));
    }
    
    // Killing Ability
    KillingAbilityTag.NetSerialize(Ar, Map, bOutSuccess);
    
    const int32 BytesUsed = Ar.Tell() - StartPos;
    #if !UE_BUILD_SHIPPING
    if (Ar.IsSaving())
    {
        UE_LOG(LogTemp, Warning, TEXT("[NetSerialize] %d bytes | Radial:%d Debuff:%d"), 
            BytesUsed, bIsRadialDamage, bIsSuccessDebuff);
    }
    #endif
    
    bOutSuccess = true;
    return true;
}