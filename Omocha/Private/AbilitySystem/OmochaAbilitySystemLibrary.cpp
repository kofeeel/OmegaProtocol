// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemGlobals.h"
#include "DataAsset/OmochaSkillData.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Character/OmochaCharacterBase.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "Engine/OverlapResult.h"
#include "Interaction/OmochaCombatInterface.h"
#include "Player/OmochaPlayerState.h" 
#include "Kismet/GameplayStatics.h"
#include "Omocha/Omocha.h"


void UOmochaAbilitySystemLibrary::SetIsRadialDamageEffectParam(FDamageEffectParams& DamageEffectParams, bool bIsRadial,
                                                               float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

// Please use this in Ability
void UOmochaAbilitySystemLibrary::ApplySkillDataToParams(FDamageEffectParams& Params, const FOmochaSkillData& SkillData,
                                                         float AbilityLevel)
{
	Params.BaseDamage = SkillData.BaseDamage * AbilityLevel;
	Params.DamageType = SkillData.DamageType;

	if (SkillData.bCanApplyKnockback)
	{
		Params.KnockbackForceMagnitude = SkillData.KnockbackStrength;
		Params.KnockbackHeightCurve = SkillData.KnockbackHeightCurve;
		Params.KnockbackSpeedCurve = SkillData.KnockbackSpeedCurve;
	}
	
	Params.DebuffType = SkillData.DebuffType;
	Params.DebuffChance = SkillData.DebuffChance;
}

bool UOmochaAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->IsBlockedHit();
	}
	return false;
}

FGameplayTag UOmochaAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		if (OmochaEffectContext->GetDamageType().IsValid())
		{
			return OmochaEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

bool UOmochaAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->IsCriticalHit();
	}
	return false;
}

float UOmochaAbilitySystemLibrary::GetSkillBaseDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaContext->GetSkillBaseDamage();
	}
	return 0.f;
}

bool UOmochaAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->IsRadialDamage();
	}
	return false;
}

float UOmochaAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

float UOmochaAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UOmochaAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

bool UOmochaAbilitySystemLibrary::IsSuccessDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->IsSuccessDebuff();
	}
	return false;
}

float UOmochaAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UOmochaAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UOmochaAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

float UOmochaAbilitySystemLibrary::GetDebuffMagnitude(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetDebuffMagnitude();
	}
	return 0.f;
}

FGameplayTag UOmochaAbilitySystemLibrary::GetDebuffType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetDebuffType();
	}
	return FGameplayTag();
}

float UOmochaAbilitySystemLibrary::GetDebuffChance(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetDebuffChance();
	}
	return 0.f;
}

FGameplayTag UOmochaAbilitySystemLibrary::GetHitType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetHitType();
	}
	return FGameplayTag();
}

FVector UOmochaAbilitySystemLibrary::GetImpulseDirection(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetImpulseDirection();
	}
	return FVector::ZeroVector;
}

float UOmochaAbilitySystemLibrary::GetImpulseMultiplier(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetImpulseMultiplier();
	}
	return 0.f;
}

FVector UOmochaAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

FGameplayTag UOmochaAbilitySystemLibrary::GetKillingAbilityTag(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<const FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return OmochaEffectContext->GetKillingAbilityTag();
	}
	return FGameplayTag();
}

void UOmochaAbilitySystemLibrary::SetHitType(FGameplayEffectContextHandle& EffectContextHandle,
                                             const FGameplayTag& HitType)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetHitType(HitType);
	}
}

void UOmochaAbilitySystemLibrary::SetImpulseDirection(FGameplayEffectContextHandle& EffectContextHandle,
                                                      const FVector& Direction)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetImpulseDirection(Direction);
	}
}

void UOmochaAbilitySystemLibrary::SetImpulseMultiplier(FGameplayEffectContextHandle& EffectContextHandle,
                                                       float Multiplier)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetImpulseMultiplier(Multiplier);
	}
}

void UOmochaAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
                                                    const FVector& InKnockbackForce)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

void UOmochaAbilitySystemLibrary::SetKillingAbilityTag(FGameplayEffectContextHandle& EffectContextHandle,
                                                       const FGameplayTag& KillingAbilityTag)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetKillingAbilityTag(KillingAbilityTag);
	}
}

void UOmochaAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,
                                                  bool bInIsBlockedHit)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UOmochaAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
                                                   bool bInIsCriticalHit)
{
	if (!EffectContextHandle.IsValid())
	{
		return;
	}
	FGameplayEffectContext* Context = EffectContextHandle.Get();

	if (!Context)
	{
		return;
	}
	FOmochaGameplayEffectContext* OmochaContext = static_cast<FOmochaGameplayEffectContext*>(Context);

	if (OmochaContext)
	{
		OmochaContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UOmochaAbilitySystemLibrary::SetSkillBaseDamage(FGameplayEffectContextHandle& EffectContextHandle,
                                                     float BaseDamage)
{
	if (FOmochaGameplayEffectContext* OmochaContext = static_cast<FOmochaGameplayEffectContext*>(EffectContextHandle.
		Get()))
	{
		OmochaContext->SetSkillBaseDamage(BaseDamage);
	}
}


void UOmochaAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle,
                                                const FGameplayTag& InDamageType)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		OmochaEffectContext->SetDamageType(InDamageType);
	}
}

void UOmochaAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
                                                    bool bInIsRadialDamage)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UOmochaAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
                                                             float InInnerRadius)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UOmochaAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
                                                             float InOuterRadius)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UOmochaAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
                                                        const FVector& InOrigin)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UOmochaAbilitySystemLibrary::SetIsSuccessDebuff(FGameplayEffectContextHandle& EffectContextHandle,
                                                     bool bInIsSuccessDebuff)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetIsSuccessDebuff(bInIsSuccessDebuff);
	}
}

void UOmochaAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle,
                                                  float InDebuffDamage)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UOmochaAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetDebuffDuration(InDuration);
	}
}

void UOmochaAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,
                                                     float InFrequency)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UOmochaAbilitySystemLibrary::SetDebuffMagnitude(FGameplayEffectContextHandle& EffectContextHandle,
	float InMagnitude)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetDebuffMagnitude(InMagnitude);
	}
}

void UOmochaAbilitySystemLibrary::SetDebuffChance(FGameplayEffectContextHandle& EffectContextHandle,
	float InChance)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetDebuffChance(InChance);
	}
}

void UOmochaAbilitySystemLibrary::SetDebuffType(FGameplayEffectContextHandle& EffectContextHandle,
                                                const FGameplayTag& InDebuffType)
{
	if (FOmochaGameplayEffectContext* OmochaEffectContext = static_cast<FOmochaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		OmochaEffectContext->SetDebuffType(InDebuffType);
	}
}

void UOmochaAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                             TArray<AActor*>& OutOverlappingActors,
                                                             const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                             const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,
	                                                             EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
		                                FCollisionObjectQueryParams(
			                                FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		                                FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			AActor* OverlappedActor = Overlap.GetActor();
			if (IsValid(OverlappedActor) && OverlappedActor->Implements<UOmochaCombatInterface>() && !
				IOmochaCombatInterface::Execute_IsDead(OverlappedActor))
			{
				OutOverlappingActors.AddUnique(IOmochaCombatInterface::Execute_GetAvatar(OverlappedActor));
			}
		}
	}
}

void UOmochaAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
                                                    TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0)
		{
			break;
		}
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UOmochaAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UOmochaAbilitySystemLibrary::ApplyDamageEffect(
	const FDamageEffectParams& DamageEffectParams)
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle =
		DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	
	if (SourceAvatarActor)
	{
		EffectContextHandle.AddInstigator(
			const_cast<AActor*>(SourceAvatarActor),
			const_cast<AActor*>(SourceAvatarActor)
			);
	}
	SetSkillBaseDamage(EffectContextHandle, DamageEffectParams.BaseDamage);
	SetHitType(EffectContextHandle, DamageEffectParams.HitType);
	SetImpulseDirection(EffectContextHandle, DamageEffectParams.ImpulseDirection);
	SetImpulseMultiplier(EffectContextHandle, DamageEffectParams.ImpulseMultiplier);
	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);
	SetKillingAbilityTag(EffectContextHandle, DamageEffectParams.KillingAbilityTag);
	SetDebuffType(EffectContextHandle, DamageEffectParams.DebuffType);
	SetDebuffChance(EffectContextHandle, DamageEffectParams.DebuffChance);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent
		->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,
		DamageEffectParams.AbilityLevel,
		EffectContextHandle
		);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType,
	                                                              DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Knockback_Force,
	                                                              DamageEffectParams.KnockbackForceMagnitude);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

void UOmochaAbilitySystemLibrary::ApplyDebuffGameplayEffect(const FEffectProperties& Props)
{
	if (!IsValid(Props.SourceASC) || !IsValid(Props.TargetASC)) return;

	FGameplayEffectContextHandle ContextHandle = Props.EffectContextHandle;
	const FGameplayTag DebuffType = GetDebuffType(ContextHandle);
	const UOmochaAbilitySystemGlobals* AsGlobals = Cast<UOmochaAbilitySystemGlobals>(&UAbilitySystemGlobals::Get());
	const TSubclassOf<UGameplayEffect>* FoundEffect = AsGlobals ? AsGlobals->DebuffEffectMap.Find(DebuffType) : nullptr;
	const TSubclassOf<UGameplayEffect> EffectToApply = (FoundEffect && *FoundEffect) ? *FoundEffect : nullptr;
	if (!EffectToApply) return;

	const float FinalDamage = GetDebuffDamage(ContextHandle);
	const float FinalDuration = GetDebuffDuration(ContextHandle);
	const float FinalMagnitude = GetDebuffMagnitude(ContextHandle);
	const float BaseFrequency = GetDebuffFrequency(ContextHandle);

	FGameplayEffectSpecHandle SpecHandle = Props.SourceASC->MakeOutgoingSpec(EffectToApply, 1.f, ContextHandle);
	if (!SpecHandle.IsValid()) return;
	
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	
	// Apply Final Data to Debuff GE
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Data_Debuff_Damage, FinalDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Data_Debuff_Duration, FinalDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Data_Debuff_Frequency, BaseFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Data_Debuff_Magnitude, FinalMagnitude);

	Props.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

FVector UOmochaAbilitySystemLibrary::AdjustLocationForWalls(const UObject* WorldContextObject,
                                                            const FVector& StartLocation, const FVector& EndLocation,
                                                            float CapsuleRadius, float CapsuleHalfHeight,
                                                            TArray<AActor*> ActorsToIgnore, bool bDrawDebug)
{
	FHitResult HitResult;
	UKismetSystemLibrary::CapsuleTraceSingle(
		WorldContextObject,
		StartLocation,
		EndLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_MoveTrace),
		false,
		ActorsToIgnore,
		bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f
	);

	if (HitResult.bBlockingHit)
	{
		FVector BlockLocation = HitResult.Location - (EndLocation - StartLocation).GetSafeNormal();
		BlockLocation.Z = 0.0f;
		return BlockLocation;
	}

	return EndLocation;
}

FGameplayTag UOmochaAbilitySystemLibrary::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UOmochaAbilitySystemLibrary::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}