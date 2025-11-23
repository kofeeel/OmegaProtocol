// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OmochaWidgetController.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "DataAsset/OmochaAbilityInfoDataAsset.h"
#include "Player/OmochaPlayerController.h"
#include "Player/OmochaPlayerState.h"

FWidgetControllerParams::FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC,
                                                 UAttributeSet* AS, AOmochaPlayerCharacter* AC)
{
	PlayerController = Cast<AOmochaPlayerController>(PC);
	PlayerState = Cast<AOmochaPlayerState>(PS);
	AbilitySystemComponent = Cast<UOmochaAbilitySystemComponent>(ASC);
	AttributeSet = Cast<UOmochaAttributeSet>(AS);
	AvatarCharacter = AC;
}

void UOmochaWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
	AvatarCharacter = WCParams.AvatarCharacter;
}

void UOmochaWidgetController::BroadcastInitialValues()
{
}

void UOmochaWidgetController::BindCallbacksToDependencies()
{
}

void UOmochaWidgetController::BroadcastAbilityInfo()
{
	AbilityClearDelegate.Broadcast();
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UOmochaAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = UOmochaAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	AbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOmochaWidgetController::BroadcastAbilityInfo(const TArray<FGameplayAbilitySpec>& AbilitySpecs)
{
	AbilityClearDelegate.Broadcast();
	
	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySpecs)
	{
		FAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UOmochaAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = UOmochaAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	}
}

FSkillBuildMessage UOmochaWidgetController::GetBuildInfo(const FGameplayTag& BuildTag)
{
	FSkillBuildData* Data = SkillBuildDataTable->FindRow<FSkillBuildData>(BuildTag.GetTagName(), TEXT(""));
	
	FSkillBuildMessage Message;
	if (Data)
	{
		Message.bSuccess = true;
		Message.Icon = Data->Icon;
		Message.Name = Data->BuildName;
		Message.Description = Data->BuildDescription;
	}
	return Message;
}

void UOmochaWidgetController::BroadcastBuildInfo(const FSkillBuildInfos& Info)
{
	BuildInfoDelegate.Broadcast(Info);
}
