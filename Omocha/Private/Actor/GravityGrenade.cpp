// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/GravityGrenade.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AGravityGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGravityGrenade, GravityFieldCenter);
}

AGravityGrenade::AGravityGrenade()
{
	GravityFieldDuration = 4.0f;
	GravityForce = 800.0f;
	GravityFieldRadius = 400.0f;
	TickInterval = 0.1f;
}

void AGravityGrenade::BeginPlay()
{
	Super::BeginPlay();
}

void AGravityGrenade::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld()) {
		GetWorld()->GetTimerManager().ClearTimer(GravityFieldTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(GravityFieldEndTimerHandle);
	}

	if (GravityFieldComponent && IsValid(GravityFieldComponent)) {
		GravityFieldComponent->DestroyComponent();
		GravityFieldComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AGravityGrenade::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	if (bHasImpacted || !HasAuthority() || !OtherActor) {
		return;
	}

	bHasImpacted = true;

	GravityFieldCenter = GetActorLocation() + GetActorLocation().UpVector * 200.0f;

	Explode();

	StartGravityField();

	Multicast_HandleExplosion(GravityFieldCenter);

	if (ProjectileMovementComponent) {
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->SetActive(false);
	}

	if (CollisionComponent) {
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (StaticMeshComponent) {
		StaticMeshComponent->SetVisibility(false);
	}
}

void AGravityGrenade::Multicast_HandleExplosion_Implementation(const FVector& ImpactLocation)
{
	Super::Multicast_HandleExplosion_Implementation(GetActorLocation());

	if (GravityFieldVFX) {
		GravityFieldComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			GravityFieldVFX,
			GravityFieldCenter,
			FRotator(90, 0, 0),
			FVector(1) * GravityFieldEffectRadius * ReplicatedRadiusScale,
			true,
			EPSCPoolMethod::None,
			true
		);
	}
	if (GravityFieldSFX) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GravityFieldSFX, GravityFieldCenter);
	}
}


void AGravityGrenade::StartGravityField()
{
	if (!HasAuthority()) {
		return;
	}

	CurrentDamageInterval = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(
		GravityFieldTimerHandle,
		this,
		&AGravityGrenade::ApplyGravityField,
		TickInterval,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		GravityFieldEndTimerHandle,
		this,
		&AGravityGrenade::EndGravityField,
		GravityFieldDuration,
		false
	);
}

void AGravityGrenade::ApplyGravityField()
{
	if (!HasAuthority()) {
		return;
	}

	// DrawDebugSphere(
	// 	GetWorld(),
	// 	GravityFieldCenter,
	// 	GravityFieldRadius * ReplicatedRadiusScale,
	// 	24,
	// 	FColor::Purple,
	// 	false,
	// 	TickInterval * 0.9f,
	// 	0,
	// 	2.0f
	// );

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (GetOwner()) {
		QueryParams.AddIgnoredActor(GetOwner());
	}

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GravityFieldCenter,
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(GravityFieldRadius * ReplicatedRadiusScale),
		QueryParams
	);

	TArray<AActor*> DamagedActors;

	for (const FOverlapResult& Overlap : OverlapResults) {
		AActor* TargetActor = Overlap.GetActor();
		if (TargetActor && CanAffectActor(TargetActor)) {
			AffectedActors.Add(TargetActor);

			ApplyGravityToActor(TargetActor);

			if (CurrentDamageInterval >= DamageInterval && !DamagedActors.Contains(TargetActor)) {
				DamagedActors.Add(TargetActor);

				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
					TargetActor)) {
					FDamageEffectParams TickDamageParams = DamageParams;
					TickDamageParams.bIsRadialDamage = true;
					TickDamageParams.RadialDamageOrigin = GravityFieldCenter;
					TickDamageParams.TargetAbilitySystemComponent = TargetASC;

					TickDamageParams.BaseDamage = GravityFieldDamagePerTick;

					UOmochaAbilitySystemLibrary::ApplyDamageEffect(TickDamageParams);
				}
			}
		}
	}

	if (CurrentDamageInterval >= DamageInterval) {
		CurrentDamageInterval = 0.0f;
	}

	CurrentDamageInterval += TickInterval;
}

void AGravityGrenade::ApplyGravityFieldDamage()
{
}

void AGravityGrenade::ApplyGravityToActor(AActor* Actor)
{
	if (!Actor) {
		return;
	}

	FVector Direction = GravityFieldCenter - Actor->GetActorLocation();
	float Distance = Direction.Size();

	if (Distance < 50.0f) {
		Direction = FVector(0, 0, 1); // 위쪽으로
		Distance = 50.0f;
	}
	else {
		Direction.Normalize();
	}

	float DistanceFactor = 1.0f - (Distance / (GravityFieldRadius * ReplicatedRadiusScale));

	DistanceFactor = FMath::Clamp(DistanceFactor, 0.0f, 1.0f);
	DistanceFactor = FMath::Pow(DistanceFactor, 0.5f);
	DistanceFactor = FMath::Max(DistanceFactor, 0.4f);

	float FinalForce = GravityForce * DistanceFactor * ReplicatedRadiusScale;
	FVector GravityVector = Direction * FinalForce;

	if (ACharacter* Character = Cast<ACharacter>(Actor)) {
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement()) {
			FVector LaunchVelocity = GravityVector * TickInterval;

			if (MovementComp->IsFalling()) {
				LaunchVelocity *= 1.5f;
			}

			Character->LaunchCharacter(LaunchVelocity, false, false);
		}
	}
	else if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent())) {
		if (PrimitiveComp->IsSimulatingPhysics()) {
			PrimitiveComp->AddForce(GravityVector * PrimitiveComp->GetMass());
		}
	}
}

void AGravityGrenade::EndGravityField()
{
	GetWorld()->GetTimerManager().ClearTimer(GravityFieldTimerHandle);

	AffectedActors.Empty();

	if (GravityFieldComponent && IsValid(GravityFieldComponent)) {
		GravityFieldComponent->DeactivateSystem();
		GravityFieldComponent->DestroyComponent();
		GravityFieldComponent = nullptr;
	}

	Destroy();
}

bool AGravityGrenade::CanAffectActor(AActor* Actor) const
{
	if (!Actor || Actor == this) {
		return false;
	}

	if (Actor == GetOwner()) {
		return false;
	}

	if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor)) {
		return false;
	}

	// TODO: Add Team Check Logic

	return true;
}
