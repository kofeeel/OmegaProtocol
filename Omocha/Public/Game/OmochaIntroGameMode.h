// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "OmochaIntroGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaIntroGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Intro")
	FName GameStartMapName = TEXT("");

	UFUNCTION()
	void OnSequenceFinished();

	UFUNCTION()
	void GoToGame();

	UFUNCTION()
	void SkipIntro();
	void ForceRestoreInputState();

	// Loading
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	float LoadingScreenDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	TArray<UTexture2D*> LoadingImages;

private:
	class ALevelSequenceActor* SequenceActor;
	bool bSkipped = false;

	UPROPERTY()
	TObjectPtr<UUserWidget> LoadingScreenWidget;

	void ShowLoadingScreen();
	void HideLoadingScreen();
	UTexture2D* GetRandomLoadingImage();
};
