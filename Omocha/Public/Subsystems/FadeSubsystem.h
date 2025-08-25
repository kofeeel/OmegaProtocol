// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "FadeSubsystem.generated.h"

UCLASS(Blueprintable)
class OMOCHA_API UFadeSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFadeSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void CheckAndCreateFadeWidget();

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void PlayFadeIn(float PlaybackSpeed = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void PlayFadeOut(float PlaybackSpeed = 1.0f);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fade|Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> FadeWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWidgetAnimation> FadeInAnim;

	UPROPERTY(Transient)
	TObjectPtr<UWidgetAnimation> FadeOutAnim;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> FadeWidgetInstance;
};
