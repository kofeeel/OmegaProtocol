#include "AbilitySystem/Abilities/BezierBarrageGamePlayAbility.h"
#include "Actor/BezierProjectile.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UBezierBarrageGamePlayAbility::UBezierBarrageGamePlayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Default values
	MissileCount = 20;
	BarrageRadius = 300.0f;
	MissileFlightDuration = 2.0f;
	ControlPointHeight = 400.0f;
	ControlPointBackDistance = 200.0f;
	MissileSpawnDelay = 0.05f;

	CurrentMissileIndex = 0;
}

void UBezierBarrageGamePlayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentMissileIndex = 0;
	SpawnedMissiles.Empty();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MissileSpawnTimer);
		World->GetTimerManager().ClearTimer(ForceEndTimer);
	}
}

void UBezierBarrageGamePlayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility,
                                               bool bWasCancelled)
{
	// Clear timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MissileSpawnTimer);
		World->GetTimerManager().ClearTimer(ForceEndTimer);
	}

	SpawnedMissiles.Empty();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBezierBarrageGamePlayAbility::FireProjectileAtTarget(const FVector& TargetLocation)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	CachedTargetLocation = TargetLocation;
	CachedCharacterLocation = Character->GetActorLocation();
	CachedCharacterRotation = Character->GetActorRotation();

	CurrentMissileIndex = 0;

	if (MissileSpawnDelay <= 0.0f)
	{
		for (int32 i = 0; i < MissileCount; i++)
		{
			CurrentMissileIndex = i;
			SpawnNextMissile();
		}
	}
	else
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(MissileSpawnTimer, this, 
				&UBezierBarrageGamePlayAbility::SpawnNextMissile, MissileSpawnDelay, true);
		}
	}
}

void UBezierBarrageGamePlayAbility::SpawnNextMissile()
{
	if (CurrentMissileIndex >= MissileCount)
	{
		// All missiles spawned, clear timer and end ability
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(MissileSpawnTimer);
		}

		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	// Calculate trajectory points using spine rotation for more realistic launch
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(Character);

	FVector StartPoint;

	StartPoint = GetLaunchLocation(Character);

	FVector ControlPoint = CalculateControlPoint(CachedCharacterLocation, Character->GetActorRotation(),
	                                             CurrentMissileIndex);
	FVector EndPoint = GenerateRandomTargetLocation(CachedTargetLocation);

	// Spawn missile
	SpawnMissileWithTrajectory(StartPoint, ControlPoint, EndPoint);

	CurrentMissileIndex++;
}

void UBezierBarrageGamePlayAbility::SpawnMissileWithTrajectory(const FVector& StartPoint, const FVector& ControlPoint,
                                                               const FVector& EndPoint)
{
    if (GetAvatarActorFromActorInfo()->HasAuthority() && ProjectileClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetAvatarActorFromActorInfo();
        SpawnParams.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FVector InitialDirection = (ControlPoint - StartPoint).GetSafeNormal();
        FRotator SpawnRotation = InitialDirection.Rotation();
        FTransform SpawnTransform(SpawnRotation, StartPoint);

        ABezierProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABezierProjectile>(
            ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
            Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

        if (Projectile)
        {
            Projectile->DamageParams = MakeDamageEffectParamsFromClassDefaults();
            Projectile->SetProCessRadius(SkillRangeData.Radius);
            
            if (ABezierProjectile* Missile = Cast<ABezierProjectile>(Projectile))
            {
                Missile->SetBezierTrajectory(StartPoint, ControlPoint, EndPoint, MissileFlightDuration);
                Missile->StartBezierFlight();
            }
            
            Projectile->FinishSpawning(SpawnTransform);
        }
    }

    if (GetAvatarActorFromActorInfo()->HasAuthority() || IsLocallyControlled())
    {
        FVector InitialDirection = (ControlPoint - StartPoint).GetSafeNormal();
        OnMuzzleEffectTriggered(StartPoint, InitialDirection);
    }
}

FVector UBezierBarrageGamePlayAbility::CalculateControlPoint(const FVector& CharacterLocation,
                                                             const FRotator& CharacterRotation,
                                                             int32 MissileIndex) const
{
	float RandomAngle = FMath::RandRange(-90.0f, 90.0f); // Random angle from -90 to +90 degrees

	// Player's forward direction as base (add 180 to get backward)
	float PlayerYaw = CharacterRotation.Yaw;
	float LaunchYaw = PlayerYaw + 180.0f + RandomAngle; // 180 degrees behind + random spread

	// Calculate launch direction
	FVector LaunchDirection = FRotator(0, LaunchYaw, 0).Vector();

	// Random distance variation for more natural spread
	float RandomDistance = FMath::RandRange(ControlPointBackDistance * 0.7f, ControlPointBackDistance * 1.3f);

	// Calculate control point position
	FVector BaseControlPoint = CharacterLocation + (LaunchDirection * ControlPointBackDistance) + FVector(
		0, 0, ControlPointHeight);

	// Add random height variation
	float HeightVariation = FMath::RandRange(-50.0f, 100.0f);
	BaseControlPoint.Z += HeightVariation;

	return BaseControlPoint;
}

FVector UBezierBarrageGamePlayAbility::GenerateRandomTargetLocation(const FVector& BaseTarget) const
{
	// Generate random offset within barrage radius using simple 2D circle
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	float RandomRadius = FMath::RandRange(0.0f, BarrageRadius);

	FVector RandomOffset = FVector(
		FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
		0.0f
	);

	return BaseTarget + RandomOffset;
}
