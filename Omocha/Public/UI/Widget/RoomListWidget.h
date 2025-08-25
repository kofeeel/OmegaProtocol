// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomListWidget.generated.h"

class UScrollBox;
class URoomWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoiningRoom, FString, Guid, FString, RoomName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelect, bool, bActivate);

/**
 * 
 */
UCLASS()
class OMOCHA_API URoomListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void CreateRoomWidget(const FOnlineSessionSearchResult& Result);
	void RemoveRoomWidgets();

	UPROPERTY(BlueprintAssignable)
	FOnJoiningRoom OnJoiningRoom;
	
protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UScrollBox* ScrollBoxRoomList;

	UFUNCTION()
	void SelectRoom(FString Guid, FString RoomName);

	UFUNCTION(BlueprintCallable)
	void EnterPressed();

	UPROPERTY(BlueprintAssignable)
	FOnSelect OnSelect;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> RoomWidgetClass;

	FString SelectedRoomId;
	
	FString SelectedRoomName;
	
	TArray<TObjectPtr<URoomWidget>> RoomWidgets;
};
