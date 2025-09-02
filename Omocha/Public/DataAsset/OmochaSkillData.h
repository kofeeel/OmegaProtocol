// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "OmochaSkillData.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct OMOCHA_API FOmochaSkillData : public FTableRowBase
{
	GENERATED_BODY()
	
	// Damage
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback")
	bool bCanApplyKnockback = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback")
	float KnockbackStrength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback")
	TSoftObjectPtr<UCurveFloat> KnockbackSpeedCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knockback")
	TSoftObjectPtr<UCurveFloat> KnockbackHeightCurve;

	// Debuff
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	FGameplayTag DebuffType = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	float DebuffChance = 100.f;
	
	FOmochaSkillData()
	{
		BaseDamage = 3.f;
		DamageType = FGameplayTag();
		bCanApplyKnockback = false;
		KnockbackStrength = 0.f;
	}
};