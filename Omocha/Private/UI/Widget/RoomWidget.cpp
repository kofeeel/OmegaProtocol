// Copyright It's My Turn


#include "UI/Widget/RoomWidget.h"

void URoomWidget::SetRoom(const FString& InGuid, const FString& InRoomName)
{
	Guid = InGuid;
	RoomName = InRoomName;
	SetComplete();
}

void URoomWidget::Selected()
{
	OnSelected.Broadcast(Guid, RoomName);
}

bool URoomWidget::IsSelected(const FString& InGuid, const FString& InRoomName) const
{
	if (Guid == InGuid && InRoomName == RoomName)
	{
		return true;
	}
	return false;
}


