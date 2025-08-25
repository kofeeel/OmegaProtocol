// Copyright It's My Turn


#include "UI/Widget/SpectatorWidget.h"

#include "Components/TextBlock.h"

void USpectatorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StatusText)
	{
		StatusText->SetText(FText::FromString(TEXT("SPECTATOR MODE")));
	}

	if (ExplanationText)
	{
		ExplanationText->SetText(FText::FromString(TEXT("Press TAB to change view")));
	}
}
