// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/OmochaUserWidget.h"

void UOmochaUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
