// Copyright It's My Turn


#include "Actor/OmochaExperienceItem.h"

#include "Character/OmochaPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Game/OmochaGameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"

AOmochaExperienceItem::AOmochaExperienceItem()
{
	PrimaryActorTick.bCanEverTick = true;

	bDestroyOnEffectApplication = true;
	bRotates = true;
	bSinusoidalMovement = true;
	RotationRate = 90.0f;
	SineAmplitude = 20.0f;

	// Collision Setting
	if (TooltipSphere)
	{
		TooltipSphere->SetSphereRadius(CollectionRange);
		TooltipSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TooltipSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void AOmochaExperienceItem::BeginPlay()
{
	Super::BeginPlay();

	StartRotation();
	StartSinusoidalMovement();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			PlayerDetectionTimer,
			this,
			&AOmochaExperienceItem::CheckForPlayers,
			DetectionInterval,
			true
		);
	}

	UE_LOG(LogTemp, Log, TEXT("Experience Item spawned with %d XP"), ExperienceAmount);
}

void AOmochaExperienceItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsHoming && IsValid(HomingTarget))
	{
		UpdateHoming(DeltaTime);
	}
}

void AOmochaExperienceItem::OnOverlap(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlap called with actor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));
    
	if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to OmochaPlayerCharacter SUCCESS"));
		GiveExperience(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to OmochaPlayerCharacter FAILED - ActorClass: %s"), 
			   TargetActor ? *TargetActor->GetClass()->GetName() : TEXT("NULL"));
	}
}

void AOmochaExperienceItem::CheckForPlayers()
{
	UE_LOG(LogTemp, Log, TEXT("CheckForPlayers called - bIsHoming: %s, HasAuthority: %s"), 
		  bIsHoming ? TEXT("true") : TEXT("false"), HasAuthority() ? TEXT("true") : TEXT("false"));
    
	if (bIsHoming || !HasAuthority()) return;

	TArray<AActor*> FoundActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		CollectionRange,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		AOmochaPlayerCharacter::StaticClass(),
		TArray<AActor*>(),
		FoundActors
	);

	UE_LOG(LogTemp, Log, TEXT("Found %d actors in range"), FoundActors.Num());

	// Closest Player find
	float ClosestDistance = CollectionRange;
	AActor* ClosestPlayer = nullptr;

	for (AActor* Actor : FoundActors)
	{
		UE_LOG(LogTemp, Log, TEXT("Checking actor: %s"), *Actor->GetName());
        
		if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(Actor))
		{
			float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
			UE_LOG(LogTemp, Log, TEXT("Player distance: %.1f"), Distance);
            
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestPlayer = Player;
			}
		}
	}

	// Homing Start
	if (ClosestPlayer)
	{
		bIsHoming = true;
		HomingTarget = ClosestPlayer;

		GetWorld()->GetTimerManager().ClearTimer(PlayerDetectionTimer);

		UE_LOG(LogTemp, Warning, TEXT("Experience Item started homing to player: %s"), *ClosestPlayer->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No valid player found for homing"));
	}
}

void AOmochaExperienceItem::UpdateHoming(float DeltaTime)
{
	if (!IsValid(HomingTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("Homing target is invalid, stopping homing"));
		bIsHoming = false;
		return;
	}

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = HomingTarget->GetActorLocation();

	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	UE_LOG(LogTemp, VeryVerbose, TEXT("Homing: Distance to target: %.1f"), Distance);

	if (Distance < 50.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached collection distance, giving experience"));
		GiveExperience(HomingTarget);
		return;
	}

	// Homing Move
	FVector NewLocation = CurrentLocation + (Direction * HomingSpeed * DeltaTime);
	SetActorLocation(NewLocation);
}

void AOmochaExperienceItem::GiveExperience(AActor* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("GiveExperience called - HasAuthority: %s"), HasAuthority() ? TEXT("true") : TEXT("false"));
    
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No authority, cannot give experience"));
		return;
	}

	if (AOmochaGameStateBase* GameState = GetWorld()->GetGameState<AOmochaGameStateBase>())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState found, adding %d XP"), ExperienceAmount);
		GameState->AddTeamXP(ExperienceAmount);

		UE_LOG(LogTemp, Warning, TEXT("Player collected %d XP! Destroying item."), ExperienceAmount);

		// TODO: Effect / Sound Play

		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameState NOT found! Cannot give experience"));
	}
}
