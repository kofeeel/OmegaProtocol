// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaUserWidget.h"
#include "StatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UStatusWidget : public UOmochaUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AppearAnim;

public:
	void PlayAppearAnimation();
	void PlayDisappearAnimation();

private:
	UFUNCTION()
	void HandleDisappearAnimationFinished();
};
