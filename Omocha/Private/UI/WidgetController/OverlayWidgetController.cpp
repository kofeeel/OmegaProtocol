// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"
#include "Player/OmochaPlayerController.h"

void UOverlayWidgetController::BroadCastInitialValues()
{
	const UOmochaAttributeSet* OmochaAttributeSet =  CastChecked<UOmochaAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(OmochaAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(OmochaAttributeSet->GetMaxHealth());
	OnOmegaChanged.Broadcast(OmochaAttributeSet->GetOmega());
	OnMaxOmegaChanged.Broadcast(OmochaAttributeSet->GetMaxOmega());
	OnPoundChargesChanged.Broadcast(OmochaAttributeSet->GetPoundCharges(), OmochaAttributeSet->GetMaxPoundCharges());

	if (UWorld* World = GetWorld())
	{
		if (AOmochaGameStateBase* GameState = Cast<AOmochaGameStateBase>(World->GetGameState()))
		{
			float XPPercent = GameState->GetXPPercent();
			float RequiredXP = GameState->GetXPRequiredForNextLevel();
			OnTeamXPChanged.Broadcast(GameState->TeamXP, RequiredXP, XPPercent);
			OnTeamLevelChanged.Broadcast(GameState->TeamLevel, GameState->TeamLevel);
		}
	}
	BroadcastAbilityInfo();

	AOmochaPlayerCharacter* PlayerCharacter = AvatarCharacter;
	if (PlayerCharacter && PlayerCharacter->WeaponComponent)
	{
		OnCurrentAmmoChanged.Broadcast(PlayerCharacter->WeaponComponent->CurrentAmmo, PlayerCharacter->WeaponComponent->MaxAmmo);
	}
	
	if (PlayerController)
	{
		PlayerController->SetCursorEnabled(true);
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UOmochaAttributeSet* OmochaAttributeSet =  CastChecked<UOmochaAttributeSet>(AttributeSet);
	if (!IsValid(AbilitySystemComponent)) return;
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetHealthAttribute())
		.AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetMaxHealthAttribute())
		.AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetOmegaAttribute())
		.AddUObject(this, &UOverlayWidgetController::OmegaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetMaxOmegaAttribute())
		.AddUObject(this, &UOverlayWidgetController::MaxOmegaChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetPoundChargesAttribute())
		.AddUObject(this, &UOverlayWidgetController::PoundChargesChanged);

	// MaxCharges도 바뀔 수 있으니 같이 바인딩
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		OmochaAttributeSet->GetMaxPoundChargesAttribute())
		.AddUObject(this, &UOverlayWidgetController::PoundChargesChanged);
	
	AbilitySystemComponent->ShieldHealthChangedDelegate.AddUObject(this, &UOverlayWidgetController::ShieldHealthChanged);

	AbilitySystemComponent->ShieldMaxHealthChangedDelegate.AddUObject(this, &UOverlayWidgetController::ShieldMaxHealthChanged);
	
	AbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);

	AbilitySystemComponent->AbilitiesSpecsGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);

	if (!IsValid(PlayerController)) return;
	
	PlayerController->OnChatMessageArrived.AddUObject(this, &UOverlayWidgetController::ChatMessageArrived);

	PlayerController->BuildInfoDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastBuildInfo);

	if (UWorld* World = GetWorld())
	{
		if (AOmochaGameStateBase* GameState = Cast<AOmochaGameStateBase>(World->GetGameState()))
		{
			GameState->OnTeamXPChanged.AddDynamic(this, &UOverlayWidgetController::OnXPChanged);
			GameState->OnTeamLevelChanged.AddDynamic(this, &UOverlayWidgetController::OnLevelChanged);
		}
		else
		{
			// Network Issue
			FTimerHandle RetryHandle;
			World->GetTimerManager().SetTimer(RetryHandle, this, &UOverlayWidgetController::BindCallbacksToDependencies, 1.0f, false);
		}
	}

	if (PlayerState)
	{
		if (AOmochaPlayerState* OmochaPS = Cast<AOmochaPlayerState>(PlayerState))
		{
			OmochaPS->OnLevelUpChoiceReady.AddDynamic(this, &UOverlayWidgetController::OnPlayerLevelUpChoices);
		}
	}

	AOmochaPlayerCharacter* PlayerCharacter = AvatarCharacter;
	if (PlayerCharacter && PlayerCharacter->WeaponComponent)
	{
		PlayerCharacter->WeaponComponent->OnCurrentAmmoChanged.AddDynamic(this, &UOverlayWidgetController::CurrentAmmoChangedCallback);
	}
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::OmegaChanged(const FOnAttributeChangeData& Data) const
{
	OnOmegaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxOmegaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxOmegaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ShieldHealthChanged(const float NewValue) const
{
	OnShieldHealthChanged.Broadcast(NewValue);
}

void UOverlayWidgetController::ShieldMaxHealthChanged(const float NewValue) const
{
	OnShieldMaxHealthChanged.Broadcast(NewValue);
}

void UOverlayWidgetController::ChatMessageArrived(const FString& Message) const
{
	OnChatMessageArrived.Broadcast(Message);
}

void UOverlayWidgetController::OnXPChanged(float CurrentXP, float RequiredXP, float XPPercent)
{
	OnTeamXPChanged.Broadcast(CurrentXP, RequiredXP, XPPercent);
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel, int32 OldLevel)
{
	OnTeamLevelChanged.Broadcast(NewLevel, OldLevel);
}

void UOverlayWidgetController::OnPlayerLevelUpChoices(const TArray<FAttributeUpgrade>& Chocies)
{
	OnLevelUpChoicesReady.Broadcast(Chocies);
}

void UOverlayWidgetController::PoundChargesChanged(const FOnAttributeChangeData& Data) const
{
	const UOmochaAttributeSet* OmochaAttributeSet = CastChecked<UOmochaAttributeSet>(AttributeSet);
	OnPoundChargesChanged.Broadcast(OmochaAttributeSet->GetPoundCharges(), OmochaAttributeSet->GetMaxPoundCharges());
}

void UOverlayWidgetController::CurrentAmmoChangedCallback(int32 NewAmmo, int32 MaxAmmo)
{
	OnCurrentAmmoChanged.Broadcast(NewAmmo, MaxAmmo);
}