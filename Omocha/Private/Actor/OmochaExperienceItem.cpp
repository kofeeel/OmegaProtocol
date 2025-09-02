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
	}

	UE_LOG(LogTemp, Warning, TEXT("Experience Item spawned (collision-based)"));
}

void AOmochaExperienceItem::BeginPlay()
{
	Super::BeginPlay();

	StartRotation();
	StartSinusoidalMovement();

	if (TooltipSphere)
	{
		TooltipSphere->OnComponentBeginOverlap.AddDynamic(this, &AOmochaExperienceItem::OnHomingRangeEnter);
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
	UE_LOG(LogTemp, Warning, TEXT("OnOverlap called with actor: %s"),
	       TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

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
	if (bIsHoming || !HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	float ClosestDistance = CollectionRange;
	AActor* ClosestPlayer = nullptr;

	for (auto Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && PC->GetPawn())
		{
			if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(PC->GetPawn()))
			{
				float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
				UE_LOG(LogTemp, Log, TEXT("Player found: %s, Distance: %.1f"), *Player->GetName(), Distance);

				if (Distance < CollectionRange && Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					ClosestPlayer = Player;
				}
			}
		}
	}

	if (ClosestPlayer)
	{
		bIsHoming = true;
		HomingTarget = ClosestPlayer;
		GetWorld()->GetTimerManager().ClearTimer(PlayerDetectionTimer);
		UE_LOG(LogTemp, Warning, TEXT("Homing started to: %s"), *ClosestPlayer->GetName());
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
	UE_LOG(LogTemp, Warning, TEXT("GiveExperience called - HasAuthority: %s"),
	       HasAuthority() ? TEXT("true") : TEXT("false"));

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

void AOmochaExperienceItem::OnHomingRangeEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult)
{
	if (bIsHoming || !HasAuthority())
	{
		return;
	}

	if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(OtherActor))
	{
		bIsHoming = true;
		HomingTarget = Player;

		UE_LOG(LogTemp, Warning, TEXT("Player entered homing range! Starting homing to: %s"), *Player->GetName());
	}
}
