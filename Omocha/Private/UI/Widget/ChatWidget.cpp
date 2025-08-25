// Copyright It's My Turn


#include "UI/Widget/ChatWidget.h"

#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerState.h"
#include "Player/OmochaPlayerController.h"
#include "UI/Widget/ChatMessageWidget.h"


void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatText)
	{
		ChatText->OnTextCommitted.AddDynamic(this, &UChatWidget::OnTextCommitted);
	}
}

void UChatWidget::AddChatMessage(UChatMessageWidget* ChatMessageWidget) const
{
	if (ChatMessageWidget)
	{
		ChatScrollBox->AddChild(ChatMessageWidget);
		ChatScrollBox->ScrollToEnd();
		ChatScrollBox->SetAnimateWheelScrolling(true);
	}
}

void UChatWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (ChatText)
		{
			FText InputText = ChatText->GetText();
			FString TrimmedText = InputText.ToString().TrimStartAndEnd();

			if (!TrimmedText.IsEmpty())
			{
				if (AOmochaPlayerController* PC= Cast<AOmochaPlayerController>(GetWorld()->GetFirstPlayerController()))
				{
					APlayerState* PS = PC->GetPlayerState<APlayerState>();
					FString Message = FString::Printf(TEXT("%s : %s"), *PS->GetPlayerName(), *TrimmedText);

					PC->Server_SendChatMessage(Message);

					ChatText->SetText(FText::GetEmpty());
				}
			}
			ChatText->SetKeyboardFocus();
		}
	}
}
