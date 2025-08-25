// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectRoom, FString, Guid, FString, RoomName);

UCLASS()
class OMOCHA_API URoomWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRoom(const FString& InGuid, const FString& InRoomName);

	UFUNCTION(BlueprintCallable)
	void Selected();

	UFUNCTION(BlueprintImplementableEvent)
	void Unselected();

	bool IsSelected(const FString& InGuid, const FString& InRoomName) const;

	UFUNCTION(BlueprintImplementableEvent)
	void SetComplete();

	FOnSelectRoom OnSelected;

protected:
	UPROPERTY(BlueprintReadOnly)
	FString Guid;

	UPROPERTY(BlueprintReadOnly)
	FString RoomName;
};
