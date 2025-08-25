// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "OmochaWeaponData.generated.h"

class UBasicAttackGameplayAbility;

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AttachmentSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTransform AttachmentTransformOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UBasicAttackGameplayAbility> BasicAttackAbility;
};
