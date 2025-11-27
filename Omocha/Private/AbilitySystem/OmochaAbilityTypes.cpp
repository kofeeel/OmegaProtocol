// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAbilityTypes.h"
#include "GameplayEffect.h"

bool FOmochaGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    // Trace Insight
    TRACE_CPUPROFILER_EVENT_SCOPE(FOmochaGameplayEffectContext::NetSerialize);
    FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

    int32 BytesWritten = 0;

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
    BytesWritten += sizeof(uint8);  // 1 byte
    
    if (Ar.IsLoading())
    {
        bIsBlockedHit = (RepBits & (1 << 0)) != 0;
        bIsCriticalHit = (RepBits & (1 << 1)) != 0;
        bIsRadialDamage = (RepBits & (1 << 2)) != 0;
        bIsSuccessDebuff = (RepBits & (1 << 3)) != 0;
    }
    
    // Damage Data
    Ar << SkillBaseDamage;
    BytesWritten += sizeof(float);  // 4 bytes
    
    DamageType.NetSerialize(Ar, Map, bOutSuccess);
    BytesWritten += 4;
    
    // Knockback Data
    bOutSuccess &= SerializePackedVector<10, 24>(KnockbackForce, Ar);
    BytesWritten += 5;  // PackedVector ~5 bytes
    
    bOutSuccess &= SerializePackedVector<10, 24>(ImpulseDirection, Ar);
    BytesWritten += 5;
    
    bOutSuccess &= SerializePackedVector<10, 24>(DeathImpulse, Ar);
    BytesWritten += 5;
    
    // Radial Damage (conditional)
    if (bIsRadialDamage)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*FString::Printf(TEXT("RadialDamage Serialize")));
        Ar << RadialDamageInnerRadius;
        Ar << RadialDamageOuterRadius;
        Ar << RadialDamageOrigin;
        BytesWritten += sizeof(float) * 2 + sizeof(FVector);  // 4 + 4 + 12 = 20
    }
    
    // Debuff Data (conditional)
    if (bIsSuccessDebuff)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*FString::Printf(TEXT("Debuff Serialize")));
        DebuffType.NetSerialize(Ar, Map, bOutSuccess);
        Ar << DebuffDuration;
        BytesWritten += sizeof(float); 
    }
    
    // HitReact Data
    Ar << ImpulseMultiplier;
    BytesWritten += sizeof(float);  // 4
    
    HitType.NetSerialize(Ar, Map, bOutSuccess);
    BytesWritten += 4;  // Tag
    
    // Killing Ability
    KillingAbilityTag.NetSerialize(Ar, Map, bOutSuccess);
    BytesWritten += 4;  // Tag
    
#if !UE_BUILD_SHIPPING
    if (Ar.IsSaving())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Custom Context] ~%d bytes | Critical:%d Radial:%d Debuff:%d"), 
            BytesWritten,
            bIsCriticalHit,
            bIsRadialDamage, 
            bIsSuccessDebuff);
    }
#endif
    
    bOutSuccess = true;
    return true;
}
