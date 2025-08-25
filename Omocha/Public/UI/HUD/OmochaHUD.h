// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OmochaHUD.generated.h"

class UChatWidget;
class UStatusWidget;
class UStatWidgetController;
class AOmochaPlayerCharacter;
class UAttributeSet;
class UAbilitySystemComponent;
class UOmochaUserWidget;
class UOverlayWidgetController;
class USettingWidgetController;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaHUD : public AHUD
{
	GENERATED_BODY()

public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS,
	                 AOmochaPlayerCharacter* AC);

	UStatWidgetController* GetStatWidgetController(const FWidgetControllerParams& WCParams);

	void ShowStatWidget();
	void HideStatWidget();

	UFUNCTION(BLueprintCallable, Category = "Settings")
	void ShowSettingsWidget();

	UFUNCTION(BLueprintCallable, Category = "Settings")
	void HideSettingsWidget();

	USettingWidgetController* GetSettingsWidgetController();

	UPROPERTY()
	TObjectPtr<UStatusWidget> StatWidget;

private:
	UPROPERTY()
	TObjectPtr<UOmochaUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOmochaUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UOmochaUserWidget> StatWidgetClass;

	UPROPERTY()
	TObjectPtr<UStatWidgetController> StatWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UStatWidgetController> StatWidgetControllerClass;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<UOmochaUserWidget> SettingsWidgetClass;
    
	UPROPERTY()
	TObjectPtr<UOmochaUserWidget> SettingsWidget;
    
	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<USettingWidgetController> SettingsWidgetControllerClass;
    
	UPROPERTY()
	TObjectPtr<USettingWidgetController> SettingsWidgetController;
};
