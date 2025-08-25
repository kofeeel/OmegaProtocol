// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/OmochaGrenade.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "Actor/Shield.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

void AOmochaGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOmochaGrenade, Radius);
	DOREPLIFETIME(AOmochaGrenade, ReplicatedRadiusScale);
	DOREPLIFETIME(AOmochaGrenade, ProcessRadius);
}

// Sets default values
AOmochaGrenade::AOmochaGrenade()
{
}

void AOmochaGrenade::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (bHasImpacted || !HasAuthority() || !OtherActor) {
		return;
	}

	if (IsValidTarget(OtherActor)) {
		if (AShield* Shield = Cast<AShield>(OtherActor)) {
			AActor* ShieldOwner = Shield->GetOwnerCharacter();
			AActor* Indicator = DamageParams.SourceAbilitySystemComponent->GetAvatarActor();

			if (ShieldOwner && Indicator && UOmochaAbilitySystemLibrary::IsNotFriend(ShieldOwner, Indicator)) {
				if (UAbilitySystemComponent* TargetASC =
					UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Shield)) {
					DamageParams.TargetAbilitySystemComponent = TargetASC;
					UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);

					StartDestroySequence();
					return;
				}
			}
			else {
				return;
			}
		}

		Explode();

		Multicast_HandleExplosion(GetActorLocation());
		StartDestroySequence();
	}
}

void AOmochaGrenade::Multicast_HandleExplosion_Implementation(const FVector& ImpactLocation)
{
	Super::Multicast_HandleImpact(ImpactLocation);
	if (ExplosionVFX) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), FRotator::ZeroRotator,
		                                         FVector(1) * ProcessRadius / Radius * ReplicatedRadiusScale);
	}
	if (NiagaraExplosionVFX) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), NiagaraExplosionVFX, GetActorLocation());
		if (NiagaraComp) {
			NiagaraComp->SetVariableFloat(FName("User.scale"), ProcessRadius / Radius * ReplicatedRadiusScale * 1.2f);
		}
	}
}

void AOmochaGrenade::OnRep_ReplicatedRadiusScale()
{
}

void AOmochaGrenade::Explode()
{
	const FVector ExplosionLocation = GetActorLocation();
	auto CurrentRadiusScale = ReplicatedRadiusScale;

	if (bShowDebug) {
		DrawDebugSphere(
			GetWorld(),
			ExplosionLocation,
			CurrentRadiusScale * ProcessRadius,
			24,
			FColor::Red,
			false,
			2.0f,
			0,
			1.0f);
	}

	DamageParams.bIsRadialDamage = true;
	DamageParams.RadialDamageOrigin = ExplosionLocation;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (GetOwner()) {
		QueryParams.AddIgnoredActor(GetOwner());
	}

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		ExplosionLocation,
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(CurrentRadiusScale * ProcessRadius),
		QueryParams
	);

	TArray<AActor*> DamagedActors;

	for (FOverlapResult& Overlap : OverlapResults) {
		AActor* TargetActor = Overlap.GetActor();

		if (TargetActor && !DamagedActors.Contains(TargetActor)) {
			DamagedActors.Add(TargetActor);

			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
				TargetActor)) {
				DamageParams.TargetAbilitySystemComponent = TargetASC;
				UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
			}
		}
	}
}
