// Copyright It's My Turn


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OmochaAttributeData.generated.h"

USTRUCT(BlueprintType)
struct OMOCHA_API FOmochaPlayerAttributeData : public FTableRowBase
{
	GENERATED_BODY()

	// Player Info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FString CharacterName = TEXT("Zenith");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FString FormType = TEXT("Normal");

	// Current Attributes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vital")
	float Health = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vital")
	float Omega = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Resilience = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float SkillDamage = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Shield = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackSpeed = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CoolDownReduction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Core")
	float Level = 1.f;

	// Critical/Dodge
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CriticalChance = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CriticalDamage = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float DodgeChance = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float KnockbackResistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSteal = 0.f;

	// Max Attributes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxHealth = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxOmega = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxMoveSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxAttackSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxCoolDownReduction = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxLevel = 99.f;

	// Character Attributes
	// Zenith
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Zenith")
	float ChainCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Zenith")
	float ChargingTimeMultiplier = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Zenith")
	float CanRotateWhileCharging = 0.f; // 0.f = false, 1.f = true

	// Vulcan
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Vulcan")
	float HasBuffRadius = 0.f; // 0.f = false, 1.f = true

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Vulcan")
	float BuffRadiusMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Vulcan")
	float UltimateBezierCount = 5;

	// Rusty
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Rusty")
	float HookSizeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Rusty")
	float HookSpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Rusty")
	float UltimateGravityPull = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Rusty")
	float ShieldMaxHealth = 0.f;

	// Common
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Common")
	float AttackPenetrationCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Common")
	float MovementSkillRangeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Specific|Common")
	float AttackProjectileSpeedMultiplier = 1.f;
};

// Enemy Attribute
USTRUCT(BlueprintType)
struct OMOCHA_API FOmochaEnemyAttributeData : public FTableRowBase
{
	GENERATED_BODY()

	// Enemy info    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FString EnemyClass = TEXT("Infantry");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FString EnemyRank = TEXT("Minion");

	// Current Attributes 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vital")
	float Health = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Resilience = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UCurveFloat* AttackDamage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float SkillDamage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UCurveFloat* Shield = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CoolDownReduction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Core")
	float Level = 1.f;

	// Critical/Dodge 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CriticalChance = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float CriticalDamage = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float DodgeChance = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float KnockbackResistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSteal = 0.f;

	// Max Attributes 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	UCurveFloat* MaxHealth = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxMoveSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxAttackSpeed = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxCoolDownReduction = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	float MaxLevel = 50.f;
};
