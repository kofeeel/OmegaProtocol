#include "AbilitySystem/Abilities/Skills/OmegaGiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "Actor/OmochaTransformProjectile.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaCharacterConfigComponent.h"
#include "Kismet/GameplayStatics.h"

UOmegaGiveAbility::UOmegaGiveAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Ability_Spacebar;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Omega_Give);
	SetAssetTags(Tags);
	DamageType = FOmochaGameplayTags::Get().Abilities_Type_None;

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

bool UOmegaGiveAbility::CheckActivationConditions() const
{
	if (!GetOwningActorFromActorInfo()) {
		return false;
	}

	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PlayerCharacter) {
		UE_LOG(LogTemp, Error, TEXT("GiveAbility PlayerCharacter Cast Failed"));
		return false;
	}

	UOmochaCharacterConfigComponent* ConfigComp = PlayerCharacter->FindComponentByClass<
		UOmochaCharacterConfigComponent>();
	if (!ConfigComp) {
		UE_LOG(LogTemp, Error, TEXT("GiveAbility ConfigComponent Not Found"));
		return false;
	}

	if (!ConfigComp->IsInOmegaState()) {
		UE_LOG(LogTemp, Error, TEXT("current state is not Omega"));
		return false;
	}

	return true;
}

void UOmegaGiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	if (!CheckActivationConditions()) {
		UE_LOG(LogTemp, Error, TEXT("GiveAbility Condition Check Failed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bHasProcessedTargetData = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	RequestGiveTargetData();
}

void UOmegaGiveAbility::RequestGiveTargetData()
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &UOmegaGiveAbility::OnGiveTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}
	
	CurrentTargetDataTask = UOmochaMouseHitTask::CreateTargetDataUnderMouseVisibility(this);
	if (CurrentTargetDataTask) {
		CurrentTargetDataTask->ValidData.AddDynamic(this, &UOmegaGiveAbility::OnGiveTargetDataReady);
		CurrentTargetDataTask->ReadyForActivation();
	}
}

void UOmegaGiveAbility::OnGiveTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (bHasProcessedTargetData) {
		return;
	}
	bHasProcessedTargetData = true;

	AActor* TargetActor = nullptr;
	FVector TargetLocation = FVector::ZeroVector;

	if (TargetDataHandle.Num() > 0) {
		const FHitResult* HitResult = TargetDataHandle.Get(0)->GetHitResult();
		if (HitResult && HitResult->bBlockingHit) {
			TargetActor = HitResult->GetActor();
			TargetLocation = HitResult->ImpactPoint;
		}
	}

	if (!TargetActor) {
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo())) {
			TargetLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 1000.0f;
		}
	}

	if (!IsValidGiveTarget(TargetActor)) {
		UE_LOG(LogTemp, Warning, TEXT("Invalid target for Give ability"));
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &UOmegaGiveAbility::OnTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	StartGiveSequence(TargetLocation, TargetActor);
}

void UOmegaGiveAbility::SpawnGiveProjectile(AActor* TargetActor)
{
	if (!TargetActor || !TransformProjectileClass) {
		UE_LOG(LogTemp, Error, TEXT("SpawnGiveProjectile: Invalid parameters"));
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	UWorld* World = GetWorld();
	if (!World) {
		return;
	}

	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator) {
		return;
	}

	FVector SpawnLocation = Instigator->GetActorLocation() + FVector(0, 0, 50);
	FRotator SpawnRotation = (TargetActor->GetActorLocation() - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.Owner = Instigator;
	SpawnParams.Instigator = Cast<APawn>(Instigator);

	AOmochaTransformProjectile* GiveProjectile = World->SpawnActorDeferred<AOmochaTransformProjectile>(
		TransformProjectileClass, FTransform(SpawnRotation, SpawnLocation), Instigator, Cast<APawn>(Instigator));

	if (GiveProjectile) {
		// Configure all properties before BeginPlay is called
		GiveProjectile->ProjectileType = ETransformProjectileType::Give;
		GiveProjectile->TargetActor = TargetActor;
		GiveProjectile->OriginalOwner = Instigator;
		GiveProjectile->bIsGiveOmegaType = true;

		// FIXED: Finish spawning to trigger BeginPlay with correct configuration
		UGameplayStatics::FinishSpawningActor(GiveProjectile, FTransform(SpawnRotation, SpawnLocation));

		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to create give projectile"));
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
	}
}

bool UOmegaGiveAbility::IsValidGiveTarget(AActor* TargetActor) const
{
	if (!TargetActor || !IsValid(TargetActor)) {
		return false;
	}

	AOmochaPlayerCharacter* TargetCharacter = Cast<AOmochaPlayerCharacter>(TargetActor);
	if (!TargetCharacter) {
		return false;
	}
	
	if (TargetCharacter == GetAvatarActorFromActorInfo())
	{
		return false;
	}
	
	UOmochaCharacterConfigComponent* ConfigComp =
		TargetCharacter->FindComponentByClass<UOmochaCharacterConfigComponent>();
	UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
	if (!ConfigComp || !TargetASC)
	{
		return false;
	}

	const bool bIsDead = TargetASC->HasMatchingGameplayTag(FOmochaGameplayTags::Get().State_Dead);
	const bool bIsNormal = ConfigComp->IsInNormalState();

	return bIsDead || bIsNormal;
}

void UOmegaGiveAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                   bool bWasCancelled)
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
