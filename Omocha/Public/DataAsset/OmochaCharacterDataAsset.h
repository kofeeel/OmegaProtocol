// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "OmochaCharacterDataAsset.generated.h"

class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Character diff tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Character Info")
	FGameplayTag CharacterTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Character Info")
	FString CharacterName;

	// Basic Abilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Normal Abilities")
	TArray<TSubclassOf<UGameplayAbility>> NormalAbilities;

	// Omega Abilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Omega Abilities")
	TArray<TSubclassOf<UGameplayAbility>> OmegaAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	USkeletalMesh* NormalMesh;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	USkeletalMesh* OmegaMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	UStaticMesh* OmegaHeadMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	FVector OmegaHeadMeshLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	FRotator OmegaHeadMeshRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")  
	FVector OmegaHeadMeshScaleOffset = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Settings")
	float NormalScale = 1.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Settings")
	float OmegaScale = 1.5f;
	
	FTransform GetOmegaHeadMeshTransform() const
	{
		return FTransform(OmegaHeadMeshRotationOffset, OmegaHeadMeshLocationOffset, OmegaHeadMeshScaleOffset);
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> NormalAttributesGE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> OmegaAttributesGE;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	TSubclassOf<UAnimInstance> NormalAnimBP;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Assets")
	TSubclassOf<UAnimInstance> OmegaAnimBP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Effects")
	UParticleSystem* TransformToOmegaEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Effects")
	UParticleSystem* TransformToNormalEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Omega Aura Effects")
	TObjectPtr<UNiagaraSystem> OmegaAuraEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Sounds")
	USoundBase* TransformToOmegaSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Sounds")
	USoundBase* TransformToNormalSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Start Animation")
	UAnimMontage* TransformToOmegaMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform Start Animation")
	UAnimMontage* TransformToNormalMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Revival Animation")
	UAnimMontage* ReviveMontage;
	
	UFUNCTION(BlueprintPure, Category = "Character Data")
	TArray<TSubclassOf<UGameplayAbility>> GetAbilitiesForState(bool bIsOmegaState) const
	{
		return bIsOmegaState ? OmegaAbilities : NormalAbilities;
	}

	UFUNCTION(BlueprintPure, Category = "Character Data")
	UParticleSystem* GetTransformEffect(bool bToOmegaState) const
	{
		return bToOmegaState ? TransformToOmegaEffect : TransformToNormalEffect;
	}

	UFUNCTION(BlueprintPure, Category = "Character Data")
	USoundBase* GetTransformSound(bool bToOmegaState) const
	{
		return bToOmegaState ? TransformToOmegaSound : TransformToNormalSound;
	}

	UFUNCTION(BlueprintPure, Category = "Character Data")
	UAnimMontage* GetTransformMontage(bool bToOmegaState) const
	{
		return bToOmegaState ? TransformToOmegaMontage : TransformToNormalMontage;
	}

	UFUNCTION(BlueprintPure, Category = "Character Data")
	float GetScaleForState(bool bIsOmegaState) const
	{
		return bIsOmegaState ? OmegaScale : NormalScale;
	}
};
