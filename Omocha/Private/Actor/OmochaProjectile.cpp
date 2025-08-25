#include "Actor/OmochaProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Actor/Shield.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Omocha/Omocha.h"
#include "Particles/ParticleSystemComponent.h"

void AOmochaProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOmochaProjectile, ProjectileMovementComponent);
}

AOmochaProjectile::AOmochaProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1500.f;
	ProjectileMovementComponent->MaxSpeed = 1500.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	ProjectileEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileEffect"));
	ProjectileEffect->SetupAttachment(RootComponent);
	ProjectileEffect->bAutoActivate = true;

	ProjectileNiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraEffect"));
	ProjectileNiagaraEffect->SetupAttachment(RootComponent);
	ProjectileNiagaraEffect->bAutoActivate = true;
	
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyChainTrace, ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_ChainTrace, ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_EnemyChainTrace, ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_ChainTrace, ECR_Ignore);
}

void AOmochaProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	RemainingPierceCount = MaxPierceCount;
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	SpawnLocation = GetActorLocation();
}

void AOmochaProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bHasImpacted && MaxTravelDistance > 0.f)
	{
		if (FVector::DistSquared(SpawnLocation, GetActorLocation()) >= FMath::Square(MaxTravelDistance))
		{
			Destroy();
		}
	}
}

void AOmochaProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	if (bHasImpacted)
	{
		return;
	}

	if (HasAuthority())
	{
		if (IsValidTarget(OtherActor))
		{
			if (AShield* Shield = Cast<AShield>(OtherActor))
			{
				AActor* ShieldOwner = Shield->GetOwnerCharacter();
				AActor* Indicator = DamageParams.SourceAbilitySystemComponent->GetAvatarActor();

				if (ShieldOwner && Indicator && UOmochaAbilitySystemLibrary::IsNotFriend(ShieldOwner, Indicator))
				{
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
						Shield))
					{
						DamageParams.TargetAbilitySystemComponent = TargetASC;
						UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
					}
				}
				else
				{
					return;
				}
			}
			else
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
					OtherActor))
				{
					DamageParams.TargetAbilitySystemComponent = TargetASC;
					UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);

					if (bCanPierce && RemainingPierceCount > 0)
					{
						HitActors.Add(OtherActor);
						RemainingPierceCount--;
						return;
					}
				}
			}
			bHasImpacted = true;
			StartDestroySequence();
		}
	}
}

void AOmochaProjectile::Multicast_HandleImpact_Implementation(const FVector& ImpactLocation)
{
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionComponent->OnComponentBeginOverlap.RemoveAll(this);
	}
	if (ProjectileEffect && IsValid(ProjectileEffect))
	{
		ProjectileEffect->Deactivate();
	}

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->SetActive(false);
	}

	OnExplode(ImpactLocation);

	// TODO : Gameplay Cue for projectile impact
	if (ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, ImpactLocation);
	}
	if (ImpactSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSFX, ImpactLocation);
	}
}

bool AOmochaProjectile::IsValidTarget(AActor* OtherActor)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return false;
	}

	AActor* MyOwner = nullptr;
	if (DamageParams.SourceAbilitySystemComponent)
	{
		MyOwner = DamageParams.SourceAbilitySystemComponent->GetAvatarActor();
	}
	else
	{
		MyOwner = GetOwner();
		if (MyOwner)
		{
			if (APawn* Pawn = Cast<APawn>(MyOwner))
			{
				MyOwner = Pawn;
			}
			else if (APlayerState* PS = Cast<APlayerState>(MyOwner))
			{
				MyOwner = PS->GetPawn();
			}
		}
	}

	if (!MyOwner || OtherActor == MyOwner)
	{
		return false;
	}

	if (HitActors.Contains(OtherActor))
	{
		return false;
	}

	// Check if it's a wall/static object - these should be valid targets for destruction
	if (!Cast<APawn>(OtherActor) && !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		return true; // Wall collision
	}

	// For pawns/characters, check friendly fire
	if (!bCanFriendlyFire && !UOmochaAbilitySystemLibrary::IsNotFriend(OtherActor, MyOwner))
	{
		return false;
	}

	return true;
}

void AOmochaProjectile::Destroyed()
{
	// TODO : GameplayCue for projectile destruction
	if (!bHasImpacted)
	{
		if (DisappearVFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DisappearVFX, GetActorLocation(), GetActorRotation());
		}
		if (DisappearSFX)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DisappearSFX, GetActorLocation());
		}
	}
    
	Super::Destroyed();
}

void AOmochaProjectile::StartDestroySequence()
{
	bHasImpacted = true;

	Multicast_HandleImpact(GetActorLocation());

	SetLifeSpan(3.0f);
}
