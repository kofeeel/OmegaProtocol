// Copyright It's My Turn


#include "Component/OmochaWeaponComponent.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "DataAsset/OmochaWeaponDataAsset.h"
#include "Character/OmochaPlayerCharacter.h"
#include "OmochaGameplayTags.h"
#include "Actor/WeaponPickupActor.h"
#include "Components/SphereComponent.h"
#include "DataAsset/OmochaWeaponData.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OmochaPlayerState.h"

void UOmochaWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOmochaWeaponComponent, EquippedWeaponGrade);
}

// Sets default values for this component's properties
UOmochaWeaponComponent::UOmochaWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultWeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultWeaponMeshComponent"));
	OtherWeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OtherWeaponMeshComponent"));
}

void UOmochaWeaponComponent::UpdateWeaponMesh(const FDataTableRowHandle& WeaponRow)
{
	if (!WeaponRow.DataTable || WeaponRow.RowName.IsNone()) {
		DefaultWeaponMeshComponent->SetStaticMesh(nullptr);
		OtherWeaponMeshComponent->SetStaticMesh(nullptr);
		return;
	}

	static const FString ContextString(TEXT("Weapon Data Context"));
	FWeaponData* WeaponData = WeaponRow.DataTable->FindRow<FWeaponData>(WeaponRow.RowName, ContextString);
	AOmochaPlayerCharacter* OwnerCharacter = Cast<AOmochaPlayerCharacter>(GetOwner());

	if (WeaponData && OwnerCharacter) {
		if (WeaponData->WeaponType == EWeaponType::Equipable)
		{
			DefaultWeaponMeshComponent->SetStaticMesh(WeaponData->DefaultWeaponMesh);
			DefaultWeaponMeshComponent->AttachToComponent(OwnerCharacter->GetMesh(),
														   FAttachmentTransformRules::SnapToTargetNotIncludingScale,
														   WeaponData->DefaultAttachmentSocketTag.GetTagName());
			DefaultWeaponMeshComponent->SetRelativeTransform(WeaponData->DefaultAttachmentTransformOffset);

			if (WeaponData->bIsDualWield && WeaponData->OtherWeaponMesh)
			{
				OtherWeaponMeshComponent->SetStaticMesh(WeaponData->OtherWeaponMesh);
				OtherWeaponMeshComponent->AttachToComponent(OwnerCharacter->GetMesh(),
														  FAttachmentTransformRules::SnapToTargetNotIncludingScale,
														  WeaponData->OtherAttachmentSocketTag.GetTagName());
				OtherWeaponMeshComponent->SetRelativeTransform(WeaponData->OtherAttachmentTransformOffset);
			}
			else
			{
				OtherWeaponMeshComponent->SetStaticMesh(nullptr);
			}
		}
		else
		{
			DefaultWeaponMeshComponent->SetStaticMesh(nullptr);
			OtherWeaponMeshComponent->SetStaticMesh(nullptr);
		}
	}
}

void UOmochaWeaponComponent::SetActiveHand(EWeaponHand Hand)
{
    ActiveHand = Hand;
}

// Called when the game starts
void UOmochaWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UOmochaWeaponComponent::Server_EquipWeapon_Implementation(const FDataTableRowHandle& WeaponDataRow, EWeaponGrade NewGrade)
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

	if (PS->EquippedWeaponRow.DataTable && !PS->EquippedWeaponRow.RowName.IsNone())
	{
		if (WeaponStatEffectHandle.IsValid()) {
			ASC->RemoveActiveGameplayEffect(WeaponStatEffectHandle);
			WeaponStatEffectHandle.Invalidate();
		}
		
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerCharacter;
			SpawnParams.Instigator = OwnerCharacter;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AWeaponPickupActor* DroppedWeapon = World->SpawnActorDeferred<AWeaponPickupActor>(
			DefaultWeaponPickupClass, OwnerCharacter->GetActorTransform(),
			OwnerCharacter,
			OwnerCharacter,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);

			if (DroppedWeapon)
			{
				DroppedWeapon->SetWeaponDataRow(PS->EquippedWeaponRow);
				DroppedWeapon->Grade = EquippedWeaponGrade;
				UGameplayStatics::FinishSpawningActor(DroppedWeapon, OwnerCharacter->GetActorTransform());

				TArray<AActor*> OverlappingActors;
				USphereComponent* InteractionSphere = DroppedWeapon->GetInteractionSphere();
				if(InteractionSphere)
				{
					InteractionSphere->GetOverlappingActors(OverlappingActors, AOmochaPlayerCharacter::StaticClass());

					for (AActor* OverlappingActor : OverlappingActors)
					{
						if (DroppedWeapon->Implements<UOmochaInteractionInterface>())
						{
							IOmochaInteractionInterface::Execute_OnBeginOverlap(DroppedWeapon, OverlappingActor);
						}
					}
				}
			}
		}
	}

	if (!WeaponDataRow.DataTable || WeaponDataRow.RowName.IsNone())
	{
		PS->EquippedWeaponRow = FDataTableRowHandle();
		
		return;
	}


	static const FString ContextString(TEXT("Weapon Data Context"));
	FWeaponData* NewWeaponData = WeaponDataRow.DataTable->FindRow<FWeaponData>(
		WeaponDataRow.RowName, ContextString);
	if (!NewWeaponData) {
		return;
	}

	if (WeaponStatEffectClass) {
		float Multiplier = 1.0f;
		switch (NewGrade) {
		case EWeaponGrade::Rare: Multiplier = 1.5f;
			break;
		case EWeaponGrade::Epic: Multiplier = 2.0f;
			break;
		case EWeaponGrade::Legendary: Multiplier = 2.5f;
			break;
		default: break;
		}

		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(WeaponStatEffectClass, 1.f, ContextHandle);

		const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

		SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.Data_AttackDamage,
		                                         NewWeaponData->AttackDamage * Multiplier);
		SpecHandle.Data->
		           SetSetByCallerMagnitude(GameplayTags.Data_AttackSpeed, NewWeaponData->AttackSpeed * Multiplier);
		SpecHandle.Data->
		           SetSetByCallerMagnitude(GameplayTags.Data_AttackRange, NewWeaponData->AttackRange * Multiplier);
		SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.Data_CriticalChance,
		                                         NewWeaponData->CriticalHitChance * Multiplier);
		SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.Data_CriticalDamage,
		                                         NewWeaponData->CriticalHitDamage * Multiplier);
		SpecHandle.Data->SetSetByCallerMagnitude(GameplayTags.Data_LifeSteal, NewWeaponData->LifeSteal * Multiplier);

		WeaponStatEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	const FGameplayTag LeftMouseButtonTag = FOmochaGameplayTags::Get().InputTag_Ability_LMB;

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities()) {
		if (Spec.DynamicAbilityTags.HasTagExact(LeftMouseButtonTag)) {
			ASC->ClearAbility(Spec.Handle);
			break;
		}
	}

	FGameplayAbilitySpec NewAbilitySpec(NewWeaponData->BasicAttackAbility);
	NewAbilitySpec.GetDynamicSpecSourceTags().AddTag(LeftMouseButtonTag);
	ASC->GiveAbility(NewAbilitySpec);

	EquippedWeaponGrade = NewGrade;
	PS->EquippedWeaponRow = WeaponDataRow;

	if (PS->GetLocalRole() == ROLE_Authority) {
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

void UOmochaWeaponComponent::EquipWeapon(const FDataTableRowHandle& WeaponDataRow, EWeaponGrade NewGrade)
{
	Server_EquipWeapon(WeaponDataRow, NewGrade);
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

UStaticMeshComponent* UOmochaWeaponComponent::GetWeaponMeshComponent() const
{
	if (ActiveHand == EWeaponHand::Left)
	{
		return OtherWeaponMeshComponent;
	}
	return DefaultWeaponMeshComponent;
}
