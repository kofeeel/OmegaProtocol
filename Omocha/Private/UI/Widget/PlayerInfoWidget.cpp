// Copyright It's My Turn


#include "UI/Widget/PlayerInfoWidget.h"
#include "Online/OmochaSessionSubsystem.h"

bool UPlayerInfoWidget::RequestSteamAvatar(const FString& SteamId)
{
	if (UTexture2D* ProfileImage = UOmochaSessionSubsystem::GetSteamAvatar(FCString::Atoi64(*SteamId)))
	{
		PlayerAvatarBrush.SetResourceObject(ProfileImage);
		PlayerAvatarBrush.ImageSize.X = ProfileImage->GetSurfaceWidth();
		PlayerAvatarBrush.ImageSize.Y = ProfileImage->GetSurfaceHeight();
		return true;
	}
	return false;
}

FSlateBrush UPlayerInfoWidget::GetPlayerAvatarBrush() const
{
	return PlayerAvatarBrush;
}
