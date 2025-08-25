// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "DataAsset/OmochaSkillData.h"
#include "AbilitySystem/OmochaAbilityTypes.h"

FDamageEffectParams UDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.AbilityLevel = GetAbilityLevel();
	Params.KillingAbilityTag = SkillTag;

	if (SkillDataTable && SkillTag.IsValid())
	{
		if (FOmochaSkillData* SkillData = SkillDataTable->FindRow<FOmochaSkillData>(SkillTag.GetTagName(), TEXT("")))
		{
			UOmochaAbilitySystemLibrary::ApplySkillDataToParams(Params, *SkillData, Params.AbilityLevel);
			Params.KnockbackHeightCurve = SkillData->KnockbackHeightCurve;
			Params.KnockbackSpeedCurve = SkillData->KnockbackSpeedCurve;
		}
	}

	if (IsValid(TargetActor) && Params.KnockbackForceMagnitude > 0.f)
	{
		Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		const FVector SourceLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		const FVector TargetLocation = TargetActor->GetActorLocation();
		const FVector KnockbackDirection = (TargetLocation - SourceLocation).GetSafeNormal();
		Params.KnockbackForce = KnockbackDirection * Params.KnockbackForceMagnitude;
	}

	return Params;
}
