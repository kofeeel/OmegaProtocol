// Copyright It's My Turn


#include "Subsystems/FadeSubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"

UFadeSubsystem::UFadeSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> FadeWidgetAsset(
		TEXT("/Game/Blueprint/UI/Widget/Fade/WBP_Fade.WBP_Fade_C"));

	if (FadeWidgetAsset.Succeeded()) {
		FadeWidgetClass = FadeWidgetAsset.Class;
	}
}

void UFadeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UFadeSubsystem::Deinitialize()
{
	if (FadeWidgetInstance) {
		FadeWidgetInstance->RemoveFromParent();
		FadeWidgetInstance = nullptr;
	}
	Super::Deinitialize();
}

void UFadeSubsystem::CheckAndCreateFadeWidget()
{
	if (IsValid(FadeWidgetInstance) && !FadeWidgetInstance->IsInViewport()) {
		FadeWidgetInstance = nullptr;
	}

	if (!IsValid(FadeWidgetInstance)) {
		if (IsValid(FadeWidgetClass)) {
			if (UWorld* World = GetWorld()) {
				FadeWidgetInstance = CreateWidget<UUserWidget>(World, FadeWidgetClass);

				if (FadeWidgetInstance) {
					FadeWidgetInstance->AddToViewport(9999);
					FadeWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

					if (UWidgetBlueprintGeneratedClass* BPClass = Cast<UWidgetBlueprintGeneratedClass>(
						FadeWidgetInstance->GetClass())) {
						for (UWidgetAnimation* Animation : BPClass->Animations) {
							if (Animation->GetFName() == TEXT("FadeInAnim_INST")) {
								FadeInAnim = Animation;
							}
							else if (Animation->GetFName() == TEXT("FadeOutAnim_INST")) {
								FadeOutAnim = Animation;
							}
						}
					}
				}
			}
		}
	}
}

void UFadeSubsystem::PlayFadeIn(float PlaybackSpeed)
{
	CheckAndCreateFadeWidget();

	if (FadeWidgetInstance && FadeInAnim) {
		FadeWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
		FadeWidgetInstance->PlayAnimation(FadeInAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
	}
}

void UFadeSubsystem::PlayFadeOut(float PlaybackSpeed)
{
	CheckAndCreateFadeWidget();

	if (FadeWidgetInstance && FadeOutAnim) {
		FadeWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
		FadeWidgetInstance->PlayAnimation(FadeOutAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
	}
}
