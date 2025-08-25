// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "DataAsset/EffectAreaData.h"
#include "OmochaAreaEffectActor.generated.h"

class USphereComponent;
class UOmochaAbilitySystemComponent;
class UOmochaAttributeSet;
class UGameplayEffect;

UCLASS()
class OMOCHA_API AOmochaAreaEffectActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	AOmochaAreaEffectActor();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClass);
	void ExecuteGameplayCueOnTarget(AActor* TargetActor, const FGameplayTag& CueTag);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> EffectSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UOmochaAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UOmochaAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle EffectAreaRowHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FGameplayTag EffectGameplayCueTag;

private:
	void InitAbilityActorInfo();
};