// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "AbilitySystem/OmochaBuildSystemLibrary.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "Character/OmochaCharacterBase.h"
#include "DataAsset/OmochaSkillData.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/OmochaCombatInterface.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "DataAsset/SkillBuildData.h"

struct OmochaDamageStatics
{

	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SkillDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(KnockbackResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Shield);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DodgeChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LifeSteal);
	
	OmochaDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, AttackDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, SkillDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, CriticalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, CriticalChance, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, Shield, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, DodgeChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, KnockbackResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UOmochaAttributeSet, LifeSteal, Source, false);
	}
};

static const OmochaDamageStatics& DamageStatics()
{
	static OmochaDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().SkillDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalChanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().ShieldDef);
	RelevantAttributesToCapture.Add(DamageStatics().DodgeChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().KnockbackResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LifeStealDef);
}

void UExecCalc_Damage::ApplyKnockback(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
								      const FGameplayEffectSpec& Spec,
								      const float FinalDamage, const bool bIsCritical) const
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	const float InitialKnockbackForceMagnitude = Spec.GetSetByCallerMagnitude(GameplayTags.Knockback_Force, false, 0.f);

	if (InitialKnockbackForceMagnitude <= 0.f) { return; }

	float ModifiedKnockbackForceMagnitude = InitialKnockbackForceMagnitude + (FinalDamage * 0.5f);
	
	FGameplayEffectContextHandle EffectContextHandle = ExecutionParams.GetOwningSpec().GetEffectContext();

	//Critical Knockback
	if (bIsCritical) { ModifiedKnockbackForceMagnitude *= 1.5f; }
	
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
        
	if (SourceASC && TargetASC)
	{
		FAggregatorEvaluateParameters EvaluateParams;
		EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
		EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

		float KnockbackResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().KnockbackResistanceDef, EvaluateParams, KnockbackResistance);

		KnockbackResistance = FMath::Clamp(KnockbackResistance, 0.f, 100.f);

		const float ResistanceMultiplier = (100.f - KnockbackResistance) / 100.f;
		const float FinalKnockbackMagnitude = ModifiedKnockbackForceMagnitude * ResistanceMultiplier;
			
		const FVector SourceLocation = SourceASC->GetAvatarActor()->GetActorLocation();
		const FVector TargetLocation = TargetASC->GetAvatarActor()->GetActorLocation();
		const FVector KnockbackDirection = (TargetLocation - SourceLocation).GetSafeNormal();
		const FVector FinalKnockbackForce = KnockbackDirection * FinalKnockbackMagnitude;
            
		UOmochaAbilitySystemLibrary::SetKnockbackForce(EffectContextHandle, FinalKnockbackForce);
	}
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                       const FGameplayEffectSpec& Spec,
                                       FAggregatorEvaluateParameters EvaluationParameters) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	if (!IsValid(SourceASC)) return;

	const FGameplayTag DebuffType = UOmochaAbilitySystemLibrary::GetDebuffType(ContextHandle);
	const FGameplayTag AbilityTag = UOmochaAbilitySystemLibrary::GetKillingAbilityTag(ContextHandle);
	if (!DebuffType.IsValid() || !AbilityTag.IsValid()) return;
	
	const AOmochaPlayerState* PS = Cast<AOmochaPlayerState>(SourceASC->GetOwnerActor());
	if (!IsValid(PS)) return;
	
	const UOmochaSkillBuildComponent* BuildComp = PS->GetSkillBuildComponent();
	if (!IsValid(BuildComp)) return;

	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	const FGameplayTag* RequiredBuildTag = GameplayTags.DebuffTypeToBuild.Find(DebuffType);

	if (RequiredBuildTag && BuildComp->HasBuild(*RequiredBuildTag))
	{
		const float DebuffChance = UOmochaAbilitySystemLibrary::GetDebuffChance(ContextHandle);
		if (FMath::RandRange(0.f, 100.f) < DebuffChance)
		{
			// Apply Build Debuff Data
			const float FinalDuration = BuildComp->GetModifiedAbilityPropertyValue(AbilityTag, GameplayTags.Property_Debuff_Duration, 0.f);
			const float FinalDamage = BuildComp->GetModifiedAbilityPropertyValue(AbilityTag, GameplayTags.Property_Debuff_Damage, 0.f);
			const float FinalMagnitude = BuildComp->GetModifiedAbilityPropertyValue(AbilityTag, GameplayTags.Property_Debuff_Magnitude, 0.f);

			//Override Debuff 
			UOmochaAbilitySystemLibrary::SetIsSuccessDebuff(ContextHandle, true);
			UOmochaAbilitySystemLibrary::SetDebuffDuration(ContextHandle, FinalDuration);
			UOmochaAbilitySystemLibrary::SetDebuffDamage(ContextHandle, FinalDamage);
			UOmochaAbilitySystemLibrary::SetDebuffMagnitude(ContextHandle, FinalMagnitude);
		}
	}
}

void UExecCalc_Damage::ApplyBuildBasedDamage(const FGameplayEffectCustomExecutionParameters& ExecutionParams, float& InOutDamage) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	if (!SourceASC) return;

	const AOmochaPlayerState* PS = Cast<AOmochaPlayerState>(SourceASC->GetOwnerActor());
	if (!PS) return;

	const UOmochaSkillBuildComponent* BuildComp = PS->GetSkillBuildComponent();
	if (!BuildComp) return;
    
	TArray<FAcquiredBuildInfo> OnHitBuilds;
	BuildComp->GetAcquiredBuildsWithCondition(EBuildTriggerCondition::OnHit, OnHitBuilds);

	for (const FAcquiredBuildInfo& BuildInfo : OnHitBuilds)
	{
		FSkillBuildData* BuildData = BuildComp->SkillBuildDataTable->FindRow<FSkillBuildData>(BuildInfo.BuildTag.GetTagName(), TEXT(""));
		if (!BuildData) continue;
        
		for (const FSkillBuildEffect& Effect : BuildData->Effects)
		{
			// Max Health Percentage Damage
			if (Effect.EffectType == EBuildEffectType::MaxHealthDamage && Effect.ValueByLevel)
			{
				const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
				if (TargetASC)
				{
					const UOmochaAttributeSet* TargetAttributeSet = Cast<UOmochaAttributeSet>(TargetASC->GetAttributeSet(UOmochaAttributeSet::StaticClass()));
					if (TargetAttributeSet)
					{
						const float MaxHealth = TargetAttributeSet->GetMaxHealth();
						const float DamagePercentage = Effect.ValueByLevel->GetFloatValue(BuildInfo.BuildLevel);
						const float AdditionalDamage = MaxHealth * (DamagePercentage / 100.0f);
						InOutDamage += AdditionalDamage;
					}
				}
			}

			// etc..
		}
	}
}


void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{		
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    if (!SourceAvatar || !TargetAvatar) return;
    if (!UOmochaAbilitySystemLibrary::IsNotFriend(SourceAvatar, TargetAvatar)) return;

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

    FGameplayEffectContextHandle EffectContextHandle = ExecutionParams.GetOwningSpec().GetEffectContext();
	float SkillBaseDamage = UOmochaAbilitySystemLibrary::GetSkillBaseDamage(EffectContextHandle);
	
    FAggregatorEvaluateParameters EvaluationParams;
    EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    float AttackDamage = 3.f;
    float SkillDamage = 1.f;
    float Shield = 1.f;
    float CriticalDamage = 1.5f;
    float CriticalChance = 0.f;
    
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDamageDef, EvaluationParams, AttackDamage);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().SkillDamageDef, EvaluationParams, SkillDamage);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ShieldDef, EvaluationParams, Shield);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalChanceDef, EvaluationParams, CriticalChance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDamageDef, EvaluationParams, CriticalDamage);
    
    CriticalChance = FMath::Max(0.f, CriticalChance);
    CriticalDamage = FMath::Max(0.f, CriticalDamage);

    float RandomFactor = FMath::RandRange(0.9f, 1.1f);

	float FinalDamage;
    if (SkillBaseDamage > 0.f) 
    {
    	//Skill Attack
        FinalDamage = (SkillBaseDamage  * (1 + SkillDamage / 100)  + AttackDamage) * RandomFactor;
    }
    else 
    {	
    	//Basic Attack
        FinalDamage = (3.f * (1 + SkillDamage / 100) + AttackDamage) * RandomFactor;
    }

    // Critical
    const bool bCriticalHit = FMath::RandRange(1, 100) <= CriticalChance;
	if (bCriticalHit) { FinalDamage *= (1 + CriticalDamage / 100); }
    
    UOmochaAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	ApplyBuildBasedDamage(ExecutionParams, FinalDamage);

	//DetermineDebuff(ExecutionParams, Spec, EvaluationParams);
	ApplyKnockback(ExecutionParams, Spec, FinalDamage, bCriticalHit);

    FinalDamage = FMath::Max(0.f, FinalDamage - Shield);
    FinalDamage = FMath::RoundToFloat(FinalDamage);
	
	float LifeSteal = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().LifeStealDef, EvaluationParams, LifeSteal);
	LifeSteal = FMath::Max<float>(0.f, LifeSteal);

	if (LifeSteal > 0.f && FinalDamage > 0.f)
	{
		const float HealAmount = FinalDamage * (LifeSteal / 100.f);

		if (SourceASC && HealAmount > 0.f)
		{
			 ExecutionParams.GetSourceAbilitySystemComponent()->ApplyModToAttribute(UOmochaAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, HealAmount);
		}
	}

	if (const FOmochaGameplayEffectContext* OmochaContext = static_cast<const FOmochaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		FGameplayTag KillingAbility = OmochaContext->GetKillingAbilityTag();
		if (KillingAbility.IsValid())
		{
			UOmochaBuildSystemLibrary::ApplyBuildsForTrigger(ExecutionParams.GetSourceAbilitySystemComponent(), ExecutionParams.GetTargetAbilitySystemComponent(), EBuildTriggerCondition::OnHit, KillingAbility);
		}
	}
    const FGameplayModifierEvaluatedData EvaluatedData(UOmochaAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
}