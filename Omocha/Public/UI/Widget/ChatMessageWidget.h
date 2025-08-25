// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessageWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class OMOCHA_API UChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChatMessageTextBlock;

	UFUNCTION(BlueprintCallable)
	void SetChatMessage(const FString& Message);
};
