// Fill out your copyright notice in the Description page of Project Settings.


#include "OmochaAssetManager.h"

#include "OmochaGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "Character/OmochaEnemy.h"
#include "DataAsset/OmochaSoundDataAsset.h"
#include "Engine/ObjectLibrary.h"

UOmochaAssetManager& UOmochaAssetManager::Get()
{
	check(GEngine)
	return *Cast<UOmochaAssetManager>(GEngine->AssetManager);
}

void UOmochaAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FOmochaGameplayTags::InitializeNativeGameplayTags();
	FOmochaEnemyNames::InitializeNames();
	UAbilitySystemGlobals::Get().InitGlobalData();

	UObjectLibrary* SoundAssetLibrary = UObjectLibrary::CreateLibrary(UOmochaSoundDataAsset::StaticClass(), false, GIsEditor);
	SoundAssetLibrary->LoadAssetDataFromPath(TEXT("/Game/Blueprint/Data/Sound"));
	SoundAssetLibrary->LoadAssetsFromAssetData();
}
