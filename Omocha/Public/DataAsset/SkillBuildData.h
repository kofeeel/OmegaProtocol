// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "SkillBuildData.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EBuildEffectType : uint8
{
	AttributeModifier UMETA(DisplayName = "Attribute Modifier"),
	GameplayEffectApplication UMETA(DisplayName = "Gameplay Effect Application"),
	AbilityProperty UMETA(DisplayName = "Ability Property Modifier"),
	GrantAbility UMETA(DisplayName = "Grant Ability"),
	MaxHealthDamage UMETA(DisplayName = "Max Health Damage"),
	CustomLogic UMETA(DisplayName = "Custom Logic")
};

// Defines when the build effect happens
UENUM(BlueprintType)
enum class EBuildTriggerCondition : uint8
{
	// No condition (it's always on)
	Passive,
	// When you use the ability
	OnAbilityActivation,
	// When you hit an enemy
	OnHit,
	// When you defeat an enemy
	OnKill,
	// When the skill misses
	OnMiss,
	// When a shield breaks
	OnShieldBroken,
	OnAbilityEnd
	// ... You can add more conditions here.
};

UENUM(BlueprintType)
enum class EEffectTarget : uint8
{
	Source,
	Target
};


// This defines a single build effect
USTRUCT(BlueprintType)
struct FSkillBuildEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBuildEffectType EffectType = EBuildEffectType::AttributeModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="EffectType != EBuildEffectType::AbilityProperty"))
	EEffectTarget EffectTarget = EEffectTarget::Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="EffectType == EBuildEffectType::AttributeModifier"))
	FGameplayAttribute AttributeToModify;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="EffectType == EBuildEffectType::AttributeModifier || EffectType == EBuildEffectType::AbilityProperty"))
	TEnumAsByte<EGameplayModOp::Type> ModificationOp = EGameplayModOp::Additive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="EffectType == EBuildEffectType::GameplayEffectApplication"))
	TSubclassOf<UGameplayEffect> EffectToApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == EBuildEffectType::GameplayEffectApplication"))
	EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::Instant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == EBuildEffectType::GameplayEffectApplication && DurationPolicy == EGameplayEffectDurationType::HasDuration"))
	TObjectPtr<UCurveFloat> DurationByLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == EBuildEffectType::GameplayEffectApplication && DurationPolicy == EGameplayEffectDurationType::HasDuration"))
	FGameplayTag DurationSetByCallerTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EffectType == EBuildEffectType::GameplayEffectApplication"))
	FGameplayTag MagnitudeSetByCallerTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="EffectType == EBuildEffectType::GrantAbility"))
	TSubclassOf<UGameplayAbility> AbilityToGrant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="EffectType == EBuildEffectType::AbilityProperty"))
	FGameplayTag PropertyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="EffectType == EBuildEffectType::CustomLogic"))
	FGameplayTag CustomLogicTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="EffectType == EBuildEffectType::AttributeModifier || EffectType == EBuildEffectType::AbilityProperty || EffectType == EBuildEffectType::GameplayEffectApplication || EffectType == EBuildEffectType::MaxHealthDamage"))
	TObjectPtr<UCurveFloat> ValueByLevel;
};

USTRUCT(BlueprintType)
struct FSkillBuildMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSuccess = false;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(BlueprintReadOnly, meta = (MultiLine = true))
	FText Description;
};

USTRUCT(BlueprintType)
struct FSkillBuildInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag BuildTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int32 BuildLevel = 0;
};

USTRUCT(BlueprintType)
struct FSkillBuildInfos
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FSkillBuildInfo> Infos;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildInfoSignature, const FSkillBuildInfos&, Info);

// The final build data structure (a row in a data table)
USTRUCT(BlueprintType)
struct FSkillBuildData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BuildTag;

	// (ex: Ability.Normal.Rusty.Skill_Q)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag TargetAbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText BuildName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText BuildDescription;

	// The condition to make it happen
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBuildTriggerCondition TriggerCondition = EBuildTriggerCondition::Passive;

	// A list of effects this build has (it can have many effects)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSkillBuildEffect> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;
};
