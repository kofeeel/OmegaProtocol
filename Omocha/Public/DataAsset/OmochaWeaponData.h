// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "OmochaWeaponData.generated.h"

UENUM(BlueprintType)
enum class EWeaponGrade : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None			UMETA(DisplayName = "None"),
	Equipable		UMETA(DisplayName = "Equippable"),
	Cosmetic		UMETA(DisplayName = "Cosmetic")
};

class UBasicAttackGameplayAbility;

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> DefaultWeaponMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponType")
	EWeaponType WeaponType = EWeaponType::Equipable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag DefaultAttachmentSocketTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform DefaultAttachmentTransformOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dual Wield")
	bool bIsDualWield = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dual Wield", meta = (EditCondition = "bIsDualWield"))
	TObjectPtr<UStaticMesh> OtherWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dual Wield", meta = (EditCondition = "bIsDualWield"))
	FGameplayTag OtherAttachmentSocketTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dual Wield", meta = (EditCondition = "bIsDualWield"))
	FTransform OtherAttachmentTransformOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UOmochaGameplayAbility> BasicAttackAbility;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 MaxAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float CriticalHitChance = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float CriticalHitDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float LifeSteal = 0.0f;
};
