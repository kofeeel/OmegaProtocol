// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterLobbyData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterLobbyData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<USkeletalMesh> TransformedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<UStaticMesh> TransformedHeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FTransform OffsetHeadTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TSubclassOf<UAnimInstance> AnimationBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TSoftObjectPtr<UParticleSystem> TransformEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	TSubclassOf<UAnimInstance> TransformedAnimationBlueprint;
};
