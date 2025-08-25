// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmochaWidgetController.generated.h"

/**
 * 
 */

class AOmochaPlayerCharacter;
struct FGameplayAbilitySpec;
class UOmochaAbilityInfoDataAsset;
class UAttributeSet;
class UAbilitySystemComponent;
class AOmochaPlayerController;
class AOmochaPlayerState;
class UOmochaAbilitySystemComponent;
class UOmochaAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilityClearSignatrue);

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}

	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS, AOmochaPlayerCharacter* AC);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AOmochaPlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AOmochaPlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UOmochaAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UOmochaAttributeSet> AttributeSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AOmochaPlayerCharacter> AvatarCharacter = nullptr;
};

UCLASS()
class OMOCHA_API UOmochaWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	
	virtual void BindCallbacksToDependencies();

	UPROPERTY(BlueprintAssignable)
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable)
	FAbilityClearSignatrue AbilityClearDelegate;

	void BroadcastAbilityInfo();

	void BroadcastAbilityInfo(const TArray<FGameplayAbilitySpec>& AbilitySpecs);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UOmochaAbilityInfoDataAsset> AbilityInfo;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AOmochaPlayerController> PlayerController = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AOmochaPlayerState> PlayerState = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<UOmochaAbilitySystemComponent> AbilitySystemComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UOmochaAttributeSet> AttributeSet = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AOmochaPlayerCharacter> AvatarCharacter = nullptr;
};
