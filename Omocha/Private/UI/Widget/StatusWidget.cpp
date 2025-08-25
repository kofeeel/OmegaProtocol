// Copyright It's My Turn


#include "UI/Widget/StatusWidget.h"

void UStatusWidget::PlayAppearAnimation()
{
	if (AppearAnim)
	{
		PlayAnimation(AppearAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	}
}

void UStatusWidget::PlayDisappearAnimation()
{
	if (AppearAnim)
	{
		/*
		FWidgetAnimationDynamicEvent FinishDelegate;
		FinishDelegate.BindDynamic(this, &UStatusWidget::HandleDisappearAnimationFinished);
		BindToAnimationFinished(AppearAnim, FinishDelegate);
		*/

		PlayAnimation(AppearAnim, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
	else
	{
		RemoveFromParent();
	}
}

void UStatusWidget::HandleDisappearAnimationFinished()
{
	RemoveFromParent();
}
