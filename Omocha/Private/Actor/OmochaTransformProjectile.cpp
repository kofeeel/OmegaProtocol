// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/OmochaTransformProjectile.h"

#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaCharacterConfigComponent.h"
#include "Game/OmochaGameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Omocha/Omocha.h"
#include "Player/OmochaPlayerController.h"

AOmochaTransformProjectile::AOmochaTransformProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bCanPierce = false;
	MaxPierceCount = 1;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->bAutoActivate = true;
	SkeletalMesh->SetIsReplicated(true);

	SkeletalMesh->SetCollisionResponseToChannel(ECC_EnemyChainTrace, ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(ECC_ChainTrace, ECR_Ignore);
	
}

void AOmochaTransformProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		if (ProjectileType == ETransformProjectileType::Give && TargetActor != nullptr) {
			DetermineProjectileType();
		}
		else if (TargetActor == nullptr) {
			ProjectileType = ETransformProjectileType::Give;
			CurrentTargetingState = ETargetingState::FallingToGround;
			bIsGiveOmegaType = true;
		}
	}
}

void AOmochaTransformProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && TargetActor != nullptr) {
		UpdateTargetingMovement(DeltaTime);
	}
}

void AOmochaTransformProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOmochaTransformProjectile, ProjectileType);
	DOREPLIFETIME(AOmochaTransformProjectile, CurrentTargetingState);
	DOREPLIFETIME(AOmochaTransformProjectile, TargetActor);
	DOREPLIFETIME(AOmochaTransformProjectile, OriginalOwner);
	DOREPLIFETIME(AOmochaTransformProjectile, StealInstigator);
}

void AOmochaTransformProjectile::DetermineProjectileType()
{
	AOmochaPlayerController* LocalPlayerController = nullptr;
	AOmochaPlayerCharacter* FireCharacter = nullptr;

	if (APlayerState* PS = Cast<APlayerState>(GetOwner())) {
		LocalPlayerController = Cast<AOmochaPlayerController>(PS->GetOwner());
	}

	if (!LocalPlayerController) {
		LocalPlayerController = Cast<AOmochaPlayerController>(GetWorld()->GetFirstPlayerController());
	}

	if (!LocalPlayerController) {
		bIsGiveOmegaType = false;
		return;
	}

	FirePlayerController = LocalPlayerController;

	if (LocalPlayerController && LocalPlayerController->GetPawn()) {
		FireCharacter = Cast<AOmochaPlayerCharacter>(LocalPlayerController->GetPawn());
	}

	if (!FireCharacter) {
		UE_LOG(LogTemp, Error, TEXT("FireCharacter not found! - Using default value"));
		bIsGiveOmegaType = false;
		return;
	}

	UOmochaCharacterConfigComponent* ConfigComp = FireCharacter->FindComponentByClass<
		UOmochaCharacterConfigComponent>();

	if (!ConfigComp) {
		UE_LOG(LogTemp, Error, TEXT("ConfigComp not found on FireCharacter! - Using default value"));
		bIsGiveOmegaType = false;
		return;
	}

	bIsGiveOmegaType = ConfigComp->IsInOmegaState();

	OriginalOwner = FireCharacter;
}

void AOmochaTransformProjectile::UpdateTargetingMovement(float DeltaTime)
{
	switch (CurrentTargetingState)
	{
	case ETargetingState::SeekingTarget:
	{
		if (IsTargetStillValid())
		{
			SeekTarget(DeltaTime);
		}
		else
		{
			FallToGround();
		}
		break;
	}

	case ETargetingState::ReturningToOwner:
	{
		if (IsOwnerStillValid())
		{
			ReturnToOwner(DeltaTime);
		}
		else
		{
			CurrentTargetingState = ETargetingState::FallingToGround;
			FallToGround();
		}
		break;
	}

	case ETargetingState::FallingToGround:
	{
		break;
	}
	}
}

void AOmochaTransformProjectile::SeekTarget(float DeltaTime)
{
	if (!TargetActor || !IsValid(TargetActor) || !ProjectileMovementComponent) {
		return;
	}

	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	FVector ToTarget = TargetLocation - CurrentLocation;

	float DistanceToTarget = ToTarget.Size();

	if (DistanceToTarget <= MinDistanceToTarget) {
		SetActorLocation(TargetLocation);

		if (AOmochaPlayerCharacter* Target = Cast<AOmochaPlayerCharacter>(TargetActor)) {
			ForceHitTarget(Target);
		}
		return;
	}

	ToTarget.Normalize();

	FVector CurrentDirection = ProjectileMovementComponent->Velocity.GetSafeNormal();
	FVector NewDirection = FMath::VInterpTo(CurrentDirection, ToTarget, DeltaTime, HomingStrength);

	ProjectileMovementComponent->Velocity = NewDirection * TargetingSpeed;

	FRotator NewRotation = FRotationMatrix::MakeFromX(NewDirection).Rotator();
	SetActorRotation(NewRotation);
}

void AOmochaTransformProjectile::ReturnToOwner(float DeltaTime)
{
	if (!OriginalOwner || !IsValid(OriginalOwner) || !ProjectileMovementComponent) {
		return;
	}

	FVector OwnerLocation = OriginalOwner->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	FVector ToOwner = OwnerLocation - CurrentLocation;

	float DistanceToOwner = ToOwner.Size();

	if (DistanceToOwner <= MinDistanceToTarget) {
		PlayTransformEffects(false, GetActorLocation());
		StartDestroySequence();
		return;
	}

	ToOwner.Normalize();
	FVector CurrentDirection = ProjectileMovementComponent->Velocity.GetSafeNormal();
	FVector NewDirection = FMath::VInterpTo(CurrentDirection, ToOwner, DeltaTime, HomingStrength);

	ProjectileMovementComponent->Velocity = NewDirection * TargetingSpeed;

	FRotator NewRotation = FRotationMatrix::MakeFromX(NewDirection).Rotator();
	SetActorRotation(NewRotation);
}

void AOmochaTransformProjectile::FallToGround()
{
	if (!ProjectileMovementComponent) {
		return;
	}

	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = FailedStateBounce;
	ProjectileMovementComponent->ProjectileGravityScale = FailedStateGravity;
	ProjectileMovementComponent->bIsHomingProjectile = false;

	ProjectileMovementComponent->Velocity *= 0.5f;

	FTimerHandle CleanupTimer;
	GetWorld()->GetTimerManager().SetTimer(CleanupTimer, [this]()
	{
		StartDestroySequence();
	}, 5.0f, false);
}

bool AOmochaTransformProjectile::IsTargetStillValid() const
{
	if (!TargetActor || !IsValid(TargetActor))
	{
		return false;
	}

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		if (TargetPawn->IsPendingKillPending() || !TargetPawn->IsValidLowLevel())
		{
			return false;
		}
	}

	return true;
}

bool AOmochaTransformProjectile::IsOwnerStillValid() const
{
	if (!OriginalOwner || !IsValid(OriginalOwner)) {
		return false;
	}

	if (APawn* OwnerPawn = Cast<APawn>(OriginalOwner)) {
		return !OwnerPawn->IsPendingKillPending() && OwnerPawn->IsValidLowLevel();
	}

	return true;
}

void AOmochaTransformProjectile::ForceHitTarget(AOmochaPlayerCharacter* Target)
{
	if (HasAuthority() && !bHasImpacted)
	{
		if (Target->GetIsDead())
		{
			bHasImpacted = true;
			// Target->Rebirth(true);
			Target->BeginReviveSequence();
			
			PlayTransformEffects(true, GetActorLocation());
			StartDestroySequence();
			return;
		}
		UOmochaCharacterConfigComponent* TargetConfigComponent = Target->FindComponentByClass<UOmochaCharacterConfigComponent>();
		if (!TargetConfigComponent)
		{
			return;
		}

		bool bTransformSuccess = false;
		if (ProjectileType == ETransformProjectileType::Give)
		{
			bTransformSuccess = HandleGiveOmega(TargetConfigComponent, Target);
		}
		else
		{
			bTransformSuccess = HandleStealOmega(TargetConfigComponent, Target);
		}

		PlayTransformEffects(bTransformSuccess, GetActorLocation());
		bHasImpacted = true;
		StartDestroySequence();
	}
}

AOmochaTransformProjectile* AOmochaTransformProjectile::CreateStealProjectile(
	UWorld* World,
	AActor* OmegaTargetActor,
	AActor* StealCaster,
	TSubclassOf<AOmochaTransformProjectile> ProjectileClass)
{
	if (!World || !OmegaTargetActor || !StealCaster || !ProjectileClass) {
		UE_LOG(LogTemp, Error, TEXT("CreateStealProjectile: Invalid parameters"));
		return nullptr;
	}

	FVector SpawnLocation = OmegaTargetActor->GetActorLocation() + FVector(0, 0, 200);
	FRotator SpawnRotation = (StealCaster->GetActorLocation() - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = StealCaster;
	SpawnParams.Instigator = Cast<APawn>(StealCaster);

	AOmochaTransformProjectile* Projectile = World->SpawnActorDeferred<AOmochaTransformProjectile>(
		ProjectileClass, FTransform(SpawnRotation, SpawnLocation), StealCaster, Cast<APawn>(StealCaster));

	if (Projectile) {
		// Configure all properties before BeginPlay is called
		Projectile->ProjectileType = ETransformProjectileType::Steal;
		Projectile->TargetActor = StealCaster;
		Projectile->OriginalOwner = OmegaTargetActor;
		Projectile->StealInstigator = StealCaster;
		Projectile->bIsGiveOmegaType = false;
		Projectile->CurrentTargetingState = ETargetingState::SeekingTarget;

		// FIXED: Finish spawning to trigger BeginPlay with correct configuration
		UGameplayStatics::FinishSpawningActor(Projectile, FTransform(SpawnRotation, SpawnLocation));
	}

	return Projectile;
}

AOmochaTransformProjectile* AOmochaTransformProjectile::SpawnAsDroppable(UWorld* World, const FVector& Location,
	AActor* Dropper, TSubclassOf<AOmochaTransformProjectile> ProjectileClass)
{
	if (!World || !Dropper || !ProjectileClass) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Dropper;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AOmochaTransformProjectile* Projectile = World->SpawnActor<AOmochaTransformProjectile>(ProjectileClass, Location, FRotator::ZeroRotator, SpawnParams);
	if (Projectile)
	{
		Projectile->ProjectileType = ETransformProjectileType::Give;
		Projectile->bIsDroppable = true;
		Projectile->OriginalOwner = Dropper;
		Projectile->TargetActor = nullptr;
		Projectile->CurrentTargetingState = ETargetingState::FallingToGround;

		if (Projectile->ProjectileMovementComponent)
		{
			Projectile->ProjectileMovementComponent->InitialSpeed = 0.f;
		}
	}
	return Projectile;
}

void AOmochaTransformProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	if (bHasImpacted || OtherActor == this || OtherActor == nullptr)
	{
		return;
	}
	
	if (CurrentTargetingState == ETargetingState::FallingToGround)
	{
		if (IsValidTransformTarget(OtherActor))
		{
			if (AOmochaPlayerCharacter* TargetCharacter = Cast<AOmochaPlayerCharacter>(OtherActor))
			{
				ForceHitTarget(TargetCharacter);
			}
		}
		return;
	}
	else if (CurrentTargetingState == ETargetingState::SeekingTarget)
	{
		if (TargetActor != OtherActor)
		{
			return;
		}

		if (IsValidTransformTarget(OtherActor))
		{
			if (HasAuthority())
			{
				if (AOmochaPlayerCharacter* TargetCharacter = Cast<AOmochaPlayerCharacter>(OtherActor))
				{
					ForceHitTarget(TargetCharacter);
				}
			}
		}
		return;
	}
}

void AOmochaTransformProjectile::StartDestroySequence()
{
	SkeletalMesh->DestroyComponent();
	
	Super::StartDestroySequence();
}

bool AOmochaTransformProjectile::HandleGiveOmega(UOmochaCharacterConfigComponent* TargetConfigComp, AActor* HitTarget)
{
	if (TargetConfigComp->IsInNormalState()) {
		TargetConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Omega);
		return true;
	}
	return false;
}

bool AOmochaTransformProjectile::HandleStealOmega(UOmochaCharacterConfigComponent* TargetConfigComp, AActor* HitTarget)
{
	if (!TargetConfigComp)
	{
		return false;
	}
	if (TargetConfigComp->IsInNormalState())
	{
		TargetConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Omega);
		return true;
	}
	
	return false;
}

void AOmochaTransformProjectile::PlayTransformEffects(bool bSuccess, const FVector& Location)
{
	if (bSuccess) {
		if (TransformSuccessVFX) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TransformSuccessVFX, Location);
		}
		if (TransformSuccessSFX) {
			UGameplayStatics::PlaySoundAtLocation(this, TransformSuccessSFX, Location);
		}
	}
	else {
		if (TransformFailVFX) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TransformFailVFX, Location);
		}
		if (TransformFailSFX) {
			UGameplayStatics::PlaySoundAtLocation(this, TransformFailSFX, Location);
		}
	}
}

void AOmochaTransformProjectile::TransformSourcePlayer(FGameplayTag TargetState)
{
	if (!FirePlayerController) {
		UE_LOG(LogTemp, Error, TEXT("SourceController is NULL"));
		return;
	}

	AOmochaPlayerCharacter* SourceCharacter = Cast<AOmochaPlayerCharacter>(FirePlayerController->GetPawn());
	if (!SourceCharacter) {
		UE_LOG(LogTemp, Error, TEXT("SourceCharacter is not a valid AOmochaPlayerCharacter: %s"),
		       FirePlayerController->GetPawn() ? *FirePlayerController->GetPawn()->GetName() : TEXT("NULL"));
		return;
	}

	UOmochaCharacterConfigComponent* SourceConfigComp =
		SourceCharacter->FindComponentByClass<UOmochaCharacterConfigComponent>();
	if (!SourceConfigComp) {
		UE_LOG(LogTemp, Error, TEXT("SourceCharacter does not have a ConfigComponent: %s"),
		       *SourceCharacter->GetName());
		return;
	}

	SourceConfigComp->ChangeState(TargetState);
}

bool AOmochaTransformProjectile::IsValidTransformTarget(AActor* TestActor)
{
	if (bIsDroppable && TestActor == OriginalOwner)
	{
		return false;
	}
	
	if (TargetActor != nullptr) {
		if (!IsValidTarget(TestActor)) {
			return false;
		}
	}
	else {
		if (TestActor == nullptr || TestActor == this) {
			return false;
		}
	}

	AOmochaPlayerCharacter* TargetCharacter = Cast<AOmochaPlayerCharacter>(TestActor);
	if (!TargetCharacter) {
		return false;
	}

	UOmochaCharacterConfigComponent* ConfigComp =
		TargetCharacter->FindComponentByClass<UOmochaCharacterConfigComponent>();
	if (!ConfigComp) {
		return false;
	}

	if (ProjectileType == ETransformProjectileType::Give) {
		return ConfigComp->IsInNormalState();
	}
	return true;
}
