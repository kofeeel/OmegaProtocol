// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaEffectActor.h"
#include "Interaction/OmochaInteractionInterface.h"
#include "UObject/Object.h"
#include "GameplayAbilitySpec.h"
#include "WeaponPickupActor.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class OMOCHA_API AWeaponPickupActor : public AOmochaEffectActor, public IOmochaInteractionInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FDataTableRowHandle WeaponDataRow;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UGameplayAbility> InteractionAbilityClass;

	FGameplayAbilitySpecHandle InteractionAbilityHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

public:
	AWeaponPickupActor();

	virtual TSubclassOf<UGameplayAbility> GetInteractionAbilityClass_Implementation() const override;
	virtual void ExecuteInteraction_Implementation(AActor* InteractingActor) override;
	virtual void OnBeginOverlap_Implementation(AActor* OverlappingActor) override;
	virtual void OnEndOverlap_Implementation(AActor* OverlappingActor) override;

	void SetWeaponDataRow(const FDataTableRowHandle& InWeaponDataRow) { WeaponDataRow = InWeaponDataRow; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentInteractable(AActor* InteractingActor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ClearCurrentInteractable(AActor* InteractingActor);

	FDataTableRowHandle GetWeaponDataRow() const { return WeaponDataRow; }

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* InteractionSphere;

	UFUNCTION()
	void OnInteractionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
