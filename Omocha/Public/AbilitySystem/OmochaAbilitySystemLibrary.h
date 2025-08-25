// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmochaAbilitySystemLibrary.generated.h"

struct FDamageEffectParams;
struct FOmochaSkillData;
struct FEffectProperties;
class UOmochaAttributeSet;
/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * Effect Context Getters
	 */

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|SkillData")
	static float GetSkillBaseDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetHitType(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static FVector GetImpulseDirection(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static float GetImpulseMultiplier(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|Knockback")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetKillingAbilityTag(const FGameplayEffectContextHandle& EffectContextHandle);

	/*
	 * Effect Context Setters
	 */

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref)
	                            FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref)
	                             FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|SkillData")
	static void SetSkillBaseDamage(UPARAM(ref)
	                               FGameplayEffectContextHandle& EffectContextHandle, float BaseDamage);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetDamageType(UPARAM(ref)
	                          FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref)
	                              FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref)
	                                       FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref)
	                                       FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref)
	                                  FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessDebuff(UPARAM(ref)
	                               FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessDebuff);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref)
	                            FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref)
	                              FGameplayEffectContextHandle& EffectContextHandle, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref)
	                               FGameplayEffectContextHandle& EffectContextHandle, float InFrequency);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetHitType(UPARAM(ref)
	                       FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& HitType);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetImpulseDirection(UPARAM(ref)
	                                FGameplayEffectContextHandle& EffectContextHandle, const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetImpulseMultiplier(UPARAM(ref)
	                                 FGameplayEffectContextHandle& EffectContextHandle, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref)
	                              FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackForce);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayEffects")
	static void SetKillingAbilityTag(UPARAM(ref)
	                                 FGameplayEffectContextHandle& EffectContextHandle,
	                                 const FGameplayTag& KillingAbilityTag);

	/*
	 * Gameplay Mechanics
	 */

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
	                                       const TArray<AActor*>& ActorsToIgnore, float Radius,
	                                       const FVector& SphereOrigin);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets,
	                              const FVector& Origin);

	UFUNCTION(BlueprintPure, Category = "OmochaAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|DamageEffect")
	static void SetIsRadialDamageEffectParam(UPARAM(ref)
	                                         FDamageEffectParams& DamageEffectParams, bool bIsRadial,
	                                         float InnerRadius, float OuterRadius, FVector Origin);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|Skill Data")
	static void ApplySkillDataToParams(UPARAM(ref)
	                                   FDamageEffectParams& Params, const FOmochaSkillData& SkillData,
	                                   float AbilityLevel = 1.f);

	static void ApplyDebuffGameplayEffect(const FEffectProperties& Props);

	UFUNCTION(BlueprintCallable, Category = "OmochaAbilitySystemLibrary|GameplayMechanics")
	static FVector AdjustLocationForWalls(const UObject* WorldContextObject, const FVector& StartLocation,
	                                      const FVector& EndLocation, float CapsuleRadius, float CapsuleHalfHeight,
	                                      TArray<AActor*> ActorsToIgnore, bool bDrawDebug);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
};
