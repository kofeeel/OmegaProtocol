// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/OmochaWidgetController.h"
#include "SettingWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OMOCHA_API USettingWidgetController : public UOmochaWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// Graphic Settings - Pure Function (Blueprint Read)
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	FIntPoint GetCurrentResolution() const;
    
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetCurrentGraphicsQuality() const;
    
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	bool IsFullscreen() const;

	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	TArray<FIntPoint> GetAvailableResolutions() const;

	// Audio Settings - Pure Function
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetCurrentMasterVolume() const;
    
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetCurrentMusicVolume() const;
    
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	float GetCurrentSFXVolume() const;

	// Gameplay Settings - Pure Function
	UFUNCTION(BlueprintPure, Category = "Settings|Gameplay")
	float GetCurrentMouseSensitivity() const;

	
	// Setting Graphics
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetResolution(FIntPoint Resolution);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFullscreenMode(bool bFullscreen);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetGraphicsQuality(int32 Quality);

	// Audio
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetSFXVolume(float Volume);

	// Gameplay
	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetMouseSensitivity(float Sensitivity);

	// Setting Save & Load
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetToDefaults();

protected:
	UPROPERTY()
	TObjectPtr<UGameUserSettings> GameUserSettings;

	UPROPERTY()
	class USoundManager* SoundManager;

private:
	float MouseSensitivity = 1.0f;
	float MasterVolume = 0.8f;
	float MusicVolume = 0.7f;
	float SFXVolume = 0.8f;
};
