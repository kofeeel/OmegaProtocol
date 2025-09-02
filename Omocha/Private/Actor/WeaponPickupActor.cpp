// Copyright It's My Turn


#include "Actor/WeaponPickupActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/PickupInteractionGameplayAbility.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "DataAsset/OmochaWeaponData.h"

void AWeaponPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponPickupActor, Grade);
}

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

	if (WeaponDataRow.DataTable && !WeaponDataRow.RowName.IsNone()) {
		static const FString ContextString(TEXT("Weapon Data Context"));
		FWeaponData* Data = WeaponDataRow.DataTable->FindRow<FWeaponData>(WeaponDataRow.RowName, ContextString);
		if (Data && Data->DefaultWeaponMesh) {
			WeaponMeshComponent->SetStaticMesh(Data->DefaultWeaponMesh);
			WeaponMeshComponent->SetRelativeScale3D(Data->DefaultAttachmentTransformOffset.GetScale3D());
		}
	}
}

void AWeaponPickupActor::ExecuteInteraction_Implementation(AActor* InteractingActor)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(InteractingActor);
	if (PlayerCharacter && PlayerCharacter->WeaponComponent && WeaponDataRow.DataTable && !WeaponDataRow.RowName.IsNone()) {
		ClearCurrentInteractable(InteractingActor);

		PlayerCharacter->WeaponComponent->EquipWeapon(WeaponDataRow, Grade);
		
		Destroy();
	}
}

void AWeaponPickupActor::OnBeginOverlap_Implementation(AActor* OverlappingActor)
{
	if (!HasAuthority()) {
		return;
	}
	UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappingActor));
	if (!ASC) {
		return;
	}

	if (!ASC->CurrentInteractable.IsValid())
	{
		ASC->CurrentInteractable = this;
	}
}

void AWeaponPickupActor::OnEndOverlap_Implementation(AActor* OverlappingActor)
{
	if (!HasAuthority()) {
		return;
	}
	UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappingActor));
	if (!ASC) {
		return;
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
