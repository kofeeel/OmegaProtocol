// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Game/OmochaGameStateBase.h"
#include "Player/OmochaPlayerState.h"
#include "UI/WidgetController/OmochaWidgetController.h"
#include "OverlayWidgetController.generated.h"

/**
 * 
 */
class UOmochaUserWidget;
class UOmochaAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageArrived, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPoundChargesChangedSignature, int32, NewCharges, int32, MaxCharges);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, MaxAmmo);


UCLASS(BlueprintType, Blueprintable)
class OMOCHA_API UOverlayWidgetController : public UOmochaWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadCastInitialValues();
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnHealthChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnHealthChangedSignature OnOmegaChanged;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnHealthChangedSignature OnMaxOmegaChanged;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnHealthChangedSignature OnShieldHealthChanged;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnHealthChangedSignature OnShieldMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category ="Chat Message")
	FOnChatMessageArrived OnChatMessageArrived;

	UPROPERTY(BlueprintAssignable, Category = "XP System")
	FOnTeamXPChanged OnTeamXPChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "XP System")
	FOnTeamLevelChanged OnTeamLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Level Up")
	FOnLevelUpChoicesReady OnLevelUpChoicesReady;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnPoundChargesChangedSignature OnPoundChargesChanged;

	UPROPERTY(BlueprintAssignable, Category ="GAS|Attributes")
	FOnAmmoChangedSignature OnCurrentAmmoChanged;
	
protected:
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void OmegaChanged(const FOnAttributeChangeData& Data) const;
	void MaxOmegaChanged(const FOnAttributeChangeData& Data) const;
	void ShieldHealthChanged(const float NewValue) const;
	void ShieldMaxHealthChanged(const float NewValue) const;
	void ChatMessageArrived(const FString& Message) const;

	UFUNCTION()
	void OnXPChanged(float CurrentXP, float RequiredXP, float XPPercent);

	UFUNCTION()
	void OnLevelChanged(int32 NewLevel, int32 OldLevel);

	UFUNCTION()
	void OnPlayerLevelUpChoices(const TArray<FAttributeUpgrade>& Chocies);

	void PoundChargesChanged(const FOnAttributeChangeData& Data) const;

	UFUNCTION()
	void CurrentAmmoChangedCallback(int32 NewAmmo, int32 MaxAmmo);
};
