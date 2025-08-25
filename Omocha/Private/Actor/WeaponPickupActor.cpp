// Copyright It's My Turn


#include "Actor/WeaponPickupActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/PickupInteractionGameplayAbility.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "DataAsset/OmochaWeaponData.h"

AWeaponPickupActor::AWeaponPickupActor()
{
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.f);

	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(RootComponent);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AWeaponPickupActor::BeginPlay()
{
	Super::BeginPlay();
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickupActor::OnInteractionSphereOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponPickupActor::OnInteractionSphereEndOverlap);

	if (WeaponDataRow.DataTable && !WeaponDataRow.RowName.IsNone())
	{
		static const FString ContextString(TEXT("Weapon Data Context"));
		FWeaponData* Data = WeaponDataRow.DataTable->FindRow<FWeaponData>(WeaponDataRow.RowName, ContextString);
		if (Data && Data->WeaponMesh)
		{
			WeaponMeshComponent->SetStaticMesh(Data->WeaponMesh);
		}
	}
}

TSubclassOf<UGameplayAbility> AWeaponPickupActor::GetInteractionAbilityClass_Implementation() const
{
	return InteractionAbilityClass;
}

void AWeaponPickupActor::ExecuteInteraction_Implementation(AActor* InteractingActor)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(InteractingActor);
	if (PlayerCharacter && WeaponDataRow.DataTable && !WeaponDataRow.RowName.IsNone()) {
		PlayerCharacter->WeaponComponent->EquipWeapon(GetClass());
		Destroy();
	}
}

void AWeaponPickupActor::OnBeginOverlap_Implementation(AActor* OverlappingActor)
{
	if (!IsValid(this)) {
		return;
	}

	if (!HasAuthority()) {
		return;
	}
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappingActor);
	if (!ASC) {
		return;
	}

	TSubclassOf<UGameplayAbility> AbilityClass = GetInteractionAbilityClass_Implementation();
	if (!AbilityClass) {
		return;
	}

	SetCurrentInteractable(OverlappingActor);

	FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
	if (const UOmochaGameplayAbility* AbilityCDO = Cast<UOmochaGameplayAbility>(AbilitySpec.Ability)) {
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityCDO->InputGameplayTag);
	}

	InteractionAbilityHandle = ASC->GiveAbility(AbilitySpec);
}

void AWeaponPickupActor::OnEndOverlap_Implementation(AActor* OverlappingActor)
{
	if (!HasAuthority()) {
		return;
	}
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappingActor);
	if (!ASC) {
		return;
	}

	if (InteractionAbilityHandle.IsValid()) {
		ASC->ClearAbility(InteractionAbilityHandle);
	}

	ClearCurrentInteractable(OverlappingActor);
}

void AWeaponPickupActor::OnInteractionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                    bool bFromSweep, const FHitResult& SweepResult)
{
	if (this->Implements<UOmochaInteractionInterface>()) {
		Execute_OnBeginOverlap(this, OtherActor);
	}
}

void AWeaponPickupActor::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (this->Implements<UOmochaInteractionInterface>()) {
		Execute_OnEndOverlap(this, OtherActor);
	}
}

void AWeaponPickupActor::SetCurrentInteractable(AActor* InteractingActor)
{
	if (UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractingActor))) {
		ASC->CurrentInteractable = this;
	}
}

void AWeaponPickupActor::ClearCurrentInteractable(AActor* InteractingActor)
{
	if (UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractingActor))) {
		if (ASC->CurrentInteractable == this) {
			ASC->CurrentInteractable = nullptr;
		}
	}
}
