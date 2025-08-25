// Copyright It's My Turn


#include "Game/OmochaIntroGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OmochaPlayerController.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"

void AOmochaIntroGameMode::BeginPlay()
{
	Super::BeginPlay();

	SequenceActor = Cast<ALevelSequenceActor>(UGameplayStatics::GetActorOfClass(this, ALevelSequenceActor::StaticClass()));

	if (SequenceActor)
	{
		ULevelSequencePlayer* Player = SequenceActor->GetSequencePlayer();
		if (Player) 
		{
			Player->OnFinished.AddDynamic(this, &AOmochaIntroGameMode::OnSequenceFinished);
			Player->Play();
       
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
			{
				FInputModeGameOnly InputModeData;
				PC->SetInputMode(InputModeData);
				PC->SetShowMouseCursor(true);
              
				PC->FlushPressedKeys();
              
				if (PC->InputComponent) 
				{
					PC->InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AOmochaIntroGameMode::SkipIntro);
				}
			}
		}
	}
	else
	{
		GoToGame();
	}
}


void AOmochaIntroGameMode::OnSequenceFinished()
{
	if (!bSkipped)
	{
		ForceRestoreInputState();
		GoToGame();
	}
}

void AOmochaIntroGameMode::GoToGame()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->FlushPressedKeys();
		
		if (UInputComponent* IC = PC->InputComponent)
		{
			IC->RemoveActionBinding(FName("Escape"), IE_Pressed);
		}
	}

	ShowLoadingScreen();
	
	FTimerHandle DelayTimer;
	GetWorldTimerManager().SetTimer(DelayTimer, [this]()
	{
	   UGameplayStatics::OpenLevel(this, GameStartMapName);
	}, LoadingScreenDuration, false);
}

void AOmochaIntroGameMode::SkipIntro()
{
	if (bSkipped) return;

	bSkipped = true;

	if (SequenceActor)
	{
		SequenceActor->GetSequencePlayer()->Stop();
	}
	ForceRestoreInputState();
	GoToGame();
}

void AOmochaIntroGameMode::ForceRestoreInputState()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();

	SlateApp.ReleaseMouseCapture();
	SlateApp.ClearKeyboardFocus(EFocusCause::Cleared);
	SlateApp.ClearUserFocus(0, EFocusCause::Cleared);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->FlushPressedKeys();
	}
}

void AOmochaIntroGameMode::ShowLoadingScreen()
{
	if (LoadingScreenWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidgetClass);
		if (LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(1000);
		}
	}
}

void AOmochaIntroGameMode::HideLoadingScreen()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
		LoadingScreenWidget = nullptr;
	}
}

UTexture2D* AOmochaIntroGameMode::GetRandomLoadingImage()
{
	if (LoadingImages.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, LoadingImages.Num() - 1);
		return LoadingImages[RandomIndex];
	}
	return nullptr;
}
