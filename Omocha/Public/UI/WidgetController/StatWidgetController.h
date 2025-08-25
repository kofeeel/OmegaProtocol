// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaWidgetController.h"
#include "StatWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChangedSignature, float, NewValue);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OMOCHA_API UStatWidgetController : public UOmochaWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadCastInitialValues();
	virtual void BindCallbacksToDependencies() override;

	// Common Stat
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnAttackDamageChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnShieldChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnMoveSpeedChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnAttackSpeedChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnCriticalChanceChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnCriticalDamageChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnStatChangedSignature OnLifeStealChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Common")
	FOnStatChangedSignature OnSkillSizeMultiplierChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Common")
	FOnStatChangedSignature OnSkillRangeMultiplierChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Common")
	FOnStatChangedSignature OnAttackPenetrationCountChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Common")
	FOnStatChangedSignature OnBulletSpeedMultiplierChanged;

	// Zenith Stat
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Zenith")
	FOnStatChangedSignature OnChainCountChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Zenith")
	FOnStatChangedSignature OnChargingTimeMultiplierChanged;

	// Vulcan Stat
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Vulcan")
	FOnStatChangedSignature OnUltimateCountChanged;
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Vulcan")
	FOnStatChangedSignature OnCanBuffRangeChanged;
	// Rusty Stat
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Rusty")
	FOnStatChangedSignature OnHookSizeMultiplierChanged;
};
