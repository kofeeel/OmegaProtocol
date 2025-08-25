// Copyright It's My Turn


#include "UI/Widget/RoomListWidget.h"

#include "OnlineSessionSettings.h"
#include "Components/ScrollBox.h"
#include "UI/Widget/RoomWidget.h"

void URoomListWidget::CreateRoomWidget(const FOnlineSessionSearchResult& Result)
{
	URoomWidget* RoomWidget = Cast<URoomWidget>(CreateWidget(this, RoomWidgetClass));
	FString RoomName, Guid;
	Result.Session.SessionSettings.Get(FName("RoomName"), RoomName);
	Result.Session.SessionSettings.Get(FName("GUID"), Guid);
	RoomWidget->SetRoom(Guid, RoomName);
	RoomWidget->OnSelected.AddDynamic(this, &ThisClass::SelectRoom);
	ScrollBoxRoomList->AddChild(RoomWidget);
	RoomWidgets.Add(RoomWidget);
}

void URoomListWidget::RemoveRoomWidgets()
{
	for (auto& Widget : RoomWidgets)
	{
		ScrollBoxRoomList->RemoveChild(Widget);
	}
	RoomWidgets.Empty();
}

void URoomListWidget::SelectRoom(FString Guid, FString RoomName)
{
	if (RoomName.Equals(SelectedRoomName) && Guid.Equals(SelectedRoomId))
	{
		SelectedRoomId.Empty();
		SelectedRoomName.Empty();
		OnSelect.Broadcast(false);
		for (auto Widget: RoomWidgets)
		{
			if (Widget->IsSelected(Guid, RoomName))
			{
				Widget->Unselected();
				break;
			}
		}
	}
	else
	{
		SelectedRoomId = Guid;
		SelectedRoomName = RoomName;
		OnSelect.Broadcast(true);
		for (auto Widget: RoomWidgets)
		{
			if (!Widget->IsSelected(Guid, RoomName))
			{
				Widget->Unselected();
				break;
			}
		}
	}
}

void URoomListWidget::EnterPressed()
{
	OnJoiningRoom.Broadcast(SelectedRoomId, SelectedRoomName);
}
