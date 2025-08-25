#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OmochaCombatInterface.generated.h"

class UNiagaraSystem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);

UINTERFACE(MinimalAPI)
class UOmochaCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OMOCHA_API IOmochaCombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetPlayerLevel() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	
	//virtual bool IsDead() const = 0;

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void ApplyKnockback(const FVector& KnockbackForce);

	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void ApplyControlledKnockback(const FVector& KnockbackForce, UCurveFloat* SpeedCurve, UCurveFloat* HeightCurve);
	
	// Add DeathImpulse
	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) = 0;
	virtual FOnDeathSignature& GetOnDeathDelegate() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsBeingHitReact() const;
	virtual bool IsBeingHitReact_Implementation() const{ return false; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetHitReactEffect() const;
	virtual UNiagaraSystem* GetHitReactEffect_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetHitReactEffectLocation() const;
	virtual FVector GetHitReactEffectLocation_Implementation() const { return FVector(); }
};
