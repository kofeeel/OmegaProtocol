// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaUserWidget.h"
#include "ChatWidget.generated.h"

class UChatMessageWidget;
/**
 * 
 */
UCLASS()
class OMOCHA_API UChatWidget : public UOmochaUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UEditableText* ChatText;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ChatScrollBox;

	UFUNCTION(BlueprintCallable)
	void AddChatMessage(UChatMessageWidget* ChatMessageWidget) const;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> ChatMessageWidgetClass;
	
	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
