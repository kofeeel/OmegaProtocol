// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/OmochaUserWidget.h"
#include "PlayerInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UPlayerInfoWidget : public UOmochaUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool RequestSteamAvatar(const FString& SteamId);
	
	UFUNCTION(BlueprintPure)
	FSlateBrush GetPlayerAvatarBrush() const;

protected:
	UPROPERTY(VisibleAnywhere, Transient)
	FSlateBrush PlayerAvatarBrush; 
};
