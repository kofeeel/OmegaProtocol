// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpectatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API USpectatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatusText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ExplanationText;
	
	virtual void NativeConstruct() override;
};
