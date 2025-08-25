// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/OmochaLobbyGameMode.h"
#include "OmochaPreviewActor.generated.h"

UCLASS()
class OMOCHA_API AOmochaPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AOmochaPreviewActor();

	UPROPERTY(EditAnywhere)
	FString PlayerTag = FString("None");
	
	UFUNCTION(BlueprintImplementableEvent)
	void ChooseCharacter(ECharacterType Type);

	UFUNCTION(BlueprintImplementableEvent)
	void GetReady(bool bReady);

protected:
	virtual void BeginPlay() override;


};
