// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "OmochaWeaponComponent.generated.h"

class UOmochaWeaponDataAsset;
class AWeaponPickupActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UOmochaWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOmochaWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AWeaponPickupActor> WeaponPickupClass);

	void UpdateWeaponMeshFromState();

	TSubclassOf<AWeaponPickupActor> GetEquippedWeaponPickupClass() const { return EquippedWeaponPickupClass; }

	UStaticMeshComponent * GetWeaponMeshComponent() const { return WeaponMeshComponent; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(TSubclassOf<AWeaponPickupActor> WeaponPickupClass);

	void UpdateWeaponMesh(const FDataTableRowHandle& WeaponRow);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	FGameplayAbilitySpecHandle CurrentBasicAttackHandle;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY(Replicated)
	TSubclassOf<AWeaponPickupActor> EquippedWeaponPickupClass;
};
