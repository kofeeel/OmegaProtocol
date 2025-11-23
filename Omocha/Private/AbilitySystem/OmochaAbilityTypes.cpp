// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAbilityTypes.h"
#include "GameplayEffect.h"

bool FOmochaGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

    // Instigator & EffectCauser 
    if (Ar.IsSaving())
    {
        AActor* InstigatorActor = Instigator.Get();
        AActor* EffectCauserActor = EffectCauser.Get();
        
        Ar << InstigatorActor;
        Ar << EffectCauserActor;
    }
    else if (Ar.IsLoading())
    {
        AActor* InstigatorActor = nullptr;
        AActor* EffectCauserActor = nullptr;
        
        Ar << InstigatorActor;
        Ar << EffectCauserActor;
        
        Instigator = InstigatorActor;
        EffectCauser = EffectCauserActor;
    }
    
    // Damage 
    Ar << SkillBaseDamage;
    Ar << bIsBlockedHit;
    Ar << bIsCriticalHit;
    
    // DamageType
    DamageType.NetSerialize(Ar, Map, bOutSuccess);
    
    Ar << DeathImpulse;
    Ar << KnockbackForce;
    Ar << ImpulseDirection;
    
    // Radial Damage
    Ar << bIsRadialDamage;
    if (bIsRadialDamage)
    {
        Ar << RadialDamageInnerRadius;
        Ar << RadialDamageOuterRadius;
        Ar << RadialDamageOrigin;
    }
    
    // Debuff
    Ar << bIsSuccessDebuff;
    if (bIsSuccessDebuff)
    {
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
    
    // HitReact
    Ar << ImpulseMultiplier;
    HitType.NetSerialize(Ar, Map, bOutSuccess);
    
    // TSoftObjectPtr
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
    
    if (Ar.IsSaving()) 
    {
        UE_LOG(LogTemp, Log, TEXT("[Server] NetSerialize Saving... Crit: %d"), bIsCriticalHit);
    }
    else if (Ar.IsLoading()) 
    {
        UE_LOG(LogTemp, Log, TEXT("[Client] NetSerialize Loading... Crit: %d"), bIsCriticalHit);
    }
    
    bOutSuccess = true;
    return true;
}