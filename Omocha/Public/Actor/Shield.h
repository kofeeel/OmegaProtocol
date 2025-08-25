// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "Shield.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldDestroyedSignatrue);

class AOmochaCharacterBase;
class UNiagaraComponent;
class UBoxComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class OMOCHA_API AShield : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	AShield();
	
	UFUNCTION(BlueprintCallable, Category = "Shield")
	bool Equip();

	UFUNCTION(BlueprintCallable, Category = "Shield")
	bool Unequip();

	UFUNCTION(BlueprintCallable, Category = "Shield")
	void SetOwnerCharacter(UObject* InOwnerCharacter);
	bool TryInitializeShield();

	ACharacter* GetOwnerCharacter();
	
	UPROPERTY(BlueprintAssignable)
	FOnShieldDestroyedSignatrue OnShieldDestroyed;

	UPROPERTY(BlueprintAssignable)
	FOnShieldDestroyedSignatrue OnShieldDamaged;
	
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void SetDamageEffectParams(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintCallable, Category = "Shield")
	void InitAbilityActorInfo();

	UFUNCTION(BlueprintCallable, Category = "Shield")
	void BroadCastInitialValues();
	
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void BindShieldHealthAttributes();

	UPROPERTY(BlueprintReadWrite, Category = "Shield")
	bool bAppliedAttributeEffect = false;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult);

	void InitializeAttributes(int32 Level) const;
	
	void Broken();

	void Damaged();

	void Repair();

	void BindCallbacks();

	FDamageEffectParams DamageParams = FDamageEffectParams();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> Shield;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{ return AbilitySystemComponent; };

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> ShieldAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Shield Effects")
	TSubclassOf<UGameplayEffect> ShieldUsingEffectClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Shield Effects")
	TSubclassOf<UGameplayEffect> ShieldBrokenEffectClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Shield Effects")
	TSubclassOf<UGameplayEffect> ShieldTimerEffectClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Shield Effects")
	TSubclassOf<UGameplayEffect> ShieldRepairedEffectClass;

	UPROPERTY()
	TObjectPtr<AOmochaCharacterBase> OwnerCharacter;

private:
	FTimerHandle DashingTimerHandle;

	bool bDashing = false;

	void StopDashing();
	void CheckNiagaraStatus();
};
