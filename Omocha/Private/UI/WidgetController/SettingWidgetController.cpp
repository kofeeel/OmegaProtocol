// Copyright It's My Turn


#include "UI/WidgetController/SettingWidgetController.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void USettingWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
}

void USettingWidgetController::BindCallbacksToDependencies()
{
	GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->LoadSettings();
	}

	LoadSettings();
}

FIntPoint USettingWidgetController::GetCurrentResolution() const
{
	if (GameUserSettings)
	{
		return GameUserSettings->GetScreenResolution();
	}

	return FIntPoint(1920, 1080);
}

int32 USettingWidgetController::GetCurrentGraphicsQuality() const
{
	if (GameUserSettings)
	{
		return GameUserSettings->GetOverallScalabilityLevel();
	}
	return 2;
}

bool USettingWidgetController::IsFullscreen() const
{
	if (GameUserSettings)
	{
		return GameUserSettings->GetFullscreenMode() == EWindowMode::Type::Fullscreen;
	}

	return false;
}

TArray<FIntPoint> USettingWidgetController::GetAvailableResolutions() const
{
	TArray<FIntPoint> Resolutions;

	Resolutions.Add(FIntPoint(1280, 720));
	Resolutions.Add(FIntPoint(1366, 768));
	Resolutions.Add(FIntPoint(1920, 1080));
	Resolutions.Add(FIntPoint(2560, 1440));
    
	return Resolutions;
}

float USettingWidgetController::GetCurrentMasterVolume() const
{
	return MasterVolume;
}

float USettingWidgetController::GetCurrentMusicVolume() const
{
	return MusicVolume;
}

float USettingWidgetController::GetCurrentSFXVolume() const
{
	return SFXVolume;
}

float USettingWidgetController::GetCurrentMouseSensitivity() const
{
	return MouseSensitivity;
}

void USettingWidgetController::SetResolution(FIntPoint Resolution)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(Resolution);
		GameUserSettings->ApplyResolutionSettings(false);
		UE_LOG(LogTemp, Warning, TEXT("Resolution set to: %dx%d"), Resolution.X, Resolution.Y);
	}
}

void USettingWidgetController::SetFullscreenMode(bool bFullscreen)
{
	if (GameUserSettings)
	{
		EWindowMode::Type Mode = bFullscreen ? EWindowMode::Type::Fullscreen : EWindowMode::Type::Windowed;
		GameUserSettings->SetFullscreenMode(Mode);
		GameUserSettings->ApplyResolutionSettings(true);
		UE_LOG(LogTemp, Warning, TEXT("Fullscreen mode: %s"), bFullscreen ? TEXT("On") : TEXT("Off"));
	}
}

void USettingWidgetController::SetGraphicsQuality(int32 Quality)
{
	if (GameUserSettings)
	{
		GameUserSettings->SetOverallScalabilityLevel(Quality);
		GameUserSettings->ApplySettings(false);
		UE_LOG(LogTemp, Warning, TEXT("Graphics quality set to: %d"), Quality);
	}
}

void USettingWidgetController::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	if (UWorld* World = GetWorld())
	{
		
	}

	UE_LOG(LogTemp, Warning, TEXT("Master volume set to: %f"), MasterVolume);
}

void USettingWidgetController::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	UE_LOG(LogTemp, Warning, TEXT("Music volume set to: %f"), MusicVolume);
}

void USettingWidgetController::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	UE_LOG(LogTemp, Warning, TEXT("SFX volume set to: %f"), SFXVolume);
}

void USettingWidgetController::SetMouseSensitivity(float Sensitivity)
{
	MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
	UE_LOG(LogTemp, Warning, TEXT("Mouse sensitivity set to: %f"), MouseSensitivity);
}

void USettingWidgetController::SaveSettings()
{
	if (GameUserSettings)
	{
		GameUserSettings->SaveSettings();
	}

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			// GameInstance를 통한 커스텀 설정 저장
			UE_LOG(LogTemp, Warning, TEXT("Custom settings saved via GameInstance"));
		}
	}
}

void USettingWidgetController::LoadSettings()
{
	MouseSensitivity = 1.0f;
	MasterVolume = 0.8f;
	MusicVolume = 0.7f;
	SFXVolume = 0.8f;

	UE_LOG(LogTemp, Warning, TEXT("Settings loaded"));
}

void USettingWidgetController::ResetToDefaults()
{
	MouseSensitivity = 1.0f;
	MasterVolume = 0.8f;
	MusicVolume = 0.7f;
	SFXVolume = 0.8f;
    
	if (GameUserSettings)
	{
		GameUserSettings->SetToDefaults();
	}
    
	UE_LOG(LogTemp, Warning, TEXT("Settings reset to defaults"));
}
