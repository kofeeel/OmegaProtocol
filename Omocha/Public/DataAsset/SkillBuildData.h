// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SkillBuildData.generated.h"

UENUM(BlueprintType)
enum class ESkillModType : uint8
{
	Add,
	Multiply,
	Override
};

USTRUCT(BlueprintType)
struct FSkillBuildData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BuildTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText BuildName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText BuildDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillModType ModType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value;
};
