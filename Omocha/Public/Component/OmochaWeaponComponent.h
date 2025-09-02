// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectTypes.h"
#include "DataAsset/OmochaWeaponData.h"
#include "OmochaWeaponComponent.generated.h"

class UGameplayEffect;
class UOmochaWeaponDataAsset;
class AWeaponPickupActor;

UENUM(BlueprintType)
enum class EWeaponHand : uint8
{
	Right,
	Left
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UOmochaWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOmochaWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(const FDataTableRowHandle& WeaponDataRow, EWeaponGrade NewGrade = EWeaponGrade::Normal);

	void UpdateWeaponMeshFromState();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetWeaponMeshComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetActiveHand(EWeaponHand Hand);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(const FDataTableRowHandle& WeaponDataRow, EWeaponGrade NewGrade = EWeaponGrade::Normal);

	void UpdateWeaponMesh(const FDataTableRowHandle& WeaponRow);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> WeaponStatEffectClass;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	FGameplayAbilitySpecHandle CurrentBasicAttackHandle;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DefaultWeaponMeshComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> OtherWeaponMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponPickupActor> DefaultWeaponPickupClass;

	FActiveGameplayEffectHandle WeaponStatEffectHandle;

	UPROPERTY(Replicated)
	EWeaponGrade EquippedWeaponGrade;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponHand ActiveHand = EWeaponHand::Right;
};
