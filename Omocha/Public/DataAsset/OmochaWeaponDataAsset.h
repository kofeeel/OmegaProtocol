// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BasicAttackGameplayAbility.h"
#include "Engine/DataAsset.h"
#include "OmochaWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName AttachmentSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UBasicAttackGameplayAbility> BasicAttackAbility;
};
