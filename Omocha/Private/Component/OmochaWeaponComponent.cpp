// Copyright It's My Turn


#include "Component/OmochaWeaponComponent.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "DataAsset/OmochaWeaponDataAsset.h"
#include "Character/OmochaPlayerCharacter.h"
#include "OmochaGameplayTags.h"
#include "Actor/WeaponPickupActor.h"
#include "DataAsset/OmochaWeaponData.h"
#include "Player/OmochaPlayerState.h"

void UOmochaWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOmochaWeaponComponent, EquippedWeaponPickupClass);
}

// Sets default values for this component's properties
UOmochaWeaponComponent::UOmochaWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
}

void UOmochaWeaponComponent::UpdateWeaponMesh(const FDataTableRowHandle& WeaponRow)
{
	if (!WeaponRow.DataTable || WeaponRow.RowName.IsNone()) {
		WeaponMeshComponent->SetStaticMesh(nullptr);
		return;
	}

	static const FString ContextString(TEXT("Weapon Data Context"));
	FWeaponData* WeaponData = WeaponRow.DataTable->FindRow<FWeaponData>(WeaponRow.RowName, ContextString);

	AOmochaPlayerCharacter* OwnerCharacter = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (WeaponData && OwnerCharacter) {
		WeaponMeshComponent->SetStaticMesh(WeaponData->WeaponMesh);
		WeaponMeshComponent->AttachToComponent(OwnerCharacter->GetMesh(),
											   FAttachmentTransformRules::SnapToTargetNotIncludingScale,
											   WeaponData->AttachmentSocketName);

		WeaponMeshComponent->SetRelativeTransform(WeaponData->AttachmentTransformOffset);
	}
}

// Called when the game starts
void UOmochaWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UOmochaWeaponComponent::Server_EquipWeapon_Implementation(TSubclassOf<AWeaponPickupActor> WeaponPickupClass)
{
	AOmochaPlayerCharacter* OwnerCharacter = Cast<AOmochaPlayerCharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }

    AOmochaPlayerState* PS = OwnerCharacter->GetPlayerState<AOmochaPlayerState>();
    if (!PS)
    {
        return;
    }
    
    UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(OwnerCharacter->GetAbilitySystemComponent());
    if (!ASC)
    {
        return;
    }

    if (EquippedWeaponPickupClass)
    {
        if (UWorld* World = GetWorld())
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = OwnerCharacter;
            SpawnParams.Instigator = OwnerCharacter;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AWeaponPickupActor* DroppedWeapon = World->SpawnActor<AWeaponPickupActor>(
                EquippedWeaponPickupClass,
                OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 100.f, // 드랍 위치
                OwnerCharacter->GetActorRotation(),
                SpawnParams
            );

            if (DroppedWeapon)
            {
                DroppedWeapon->SetWeaponDataRow(PS->EquippedWeaponRow);
            }
        }
    }

    if (!WeaponPickupClass)
    {
        EquippedWeaponPickupClass = nullptr;
        PS->EquippedWeaponRow = FDataTableRowHandle();
        return;
    }

    const AWeaponPickupActor* PickupCDO = WeaponPickupClass.GetDefaultObject();
	const FDataTableRowHandle NewWeaponDataRow = PickupCDO->GetWeaponDataRow(); 

    if (!NewWeaponDataRow.DataTable || NewWeaponDataRow.RowName.IsNone())
    {
        return;
    }

    static const FString ContextString(TEXT("Weapon Data Context"));
    FWeaponData* NewWeaponData = NewWeaponDataRow.DataTable->FindRow<FWeaponData>(NewWeaponDataRow.RowName, ContextString);
    if (!NewWeaponData)
    {
        return;
    }

    const FGameplayTag LeftMouseButtonTag = FOmochaGameplayTags::Get().InputTag_Ability_LMB;
    
    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.DynamicAbilityTags.HasTagExact(LeftMouseButtonTag))
        {
            ASC->ClearAbility(Spec.Handle);
            break; 
        }
    }
    
    FGameplayAbilitySpec NewAbilitySpec(NewWeaponData->BasicAttackAbility);
    NewAbilitySpec.GetDynamicSpecSourceTags().AddTag(LeftMouseButtonTag);
    ASC->GiveAbility(NewAbilitySpec);

    EquippedWeaponPickupClass = WeaponPickupClass;
    PS->EquippedWeaponRow = NewWeaponDataRow;

    if (PS->GetLocalRole() == ROLE_Authority)
    {
        PS->OnRep_EquippedWeapon();
    }
}

// Called every frame
void UOmochaWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOmochaWeaponComponent::EquipWeapon(TSubclassOf<AWeaponPickupActor> WeaponPickupClass)
{
    Server_EquipWeapon(WeaponPickupClass);
}

void UOmochaWeaponComponent::UpdateWeaponMeshFromState()
{
	AOmochaPlayerCharacter* OwnerCharacter = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (OwnerCharacter) {
		AOmochaPlayerState* PS = OwnerCharacter->GetPlayerState<AOmochaPlayerState>();
		if (PS) {
			UpdateWeaponMesh(PS->EquippedWeaponRow);
		}
	}
}
