// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/Texture2D.h"
#include "OmochaLevelUpTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct OMOCHA_API FAttributeUpgrade
{
	GENERATED_BODY()

	FAttributeUpgrade()
	{
		AttributeTag = FGameplayTag();
		UpgradeAmount = 0.0f;
		DisplayName = FText();
		Description = FText();
		Icon = nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
	float UpgradeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* Icon;
};
