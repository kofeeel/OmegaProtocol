// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "OmochaAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UOmochaAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
