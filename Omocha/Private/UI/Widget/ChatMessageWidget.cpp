// Copyright It's My Turn


#include "UI/Widget/ChatMessageWidget.h"

#include "Components/TextBlock.h"

void UChatMessageWidget::SetChatMessage(const FString& Message)
{
	if (ChatMessageTextBlock)
	{
		ChatMessageTextBlock->SetText(FText::FromString(Message));
	}
}
