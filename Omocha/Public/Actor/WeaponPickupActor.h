// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaEffectActor.h"
#include "Interaction/OmochaInteractionInterface.h"
#include "DataAsset/OmochaWeaponData.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

public:
	AWeaponPickupActor();

	USphereComponent* GetInteractionSphere() const { return InteractionSphere; }

	virtual void ExecuteInteraction_Implementation(AActor* InteractingActor) override;
	virtual void OnBeginOverlap_Implementation(AActor* OverlappingActor) override;
	virtual void OnEndOverlap_Implementation(AActor* OverlappingActor) override;

	void SetWeaponDataRow(const FDataTableRowHandle& InWeaponDataRow) { WeaponDataRow = InWeaponDataRow; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentInteractable(AActor* InteractingActor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ClearCurrentInteractable(AActor* InteractingActor);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponGrade Grade = EWeaponGrade::Normal;

	FDataTableRowHandle GetWeaponDataRow() const { return WeaponDataRow; }

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionSphere;

	UFUNCTION()
	void OnInteractionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
