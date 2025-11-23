// Copyright It's My Turn


#include "Actor/OmochaEXPActor.h"

#include "Character/OmochaPlayerCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AOmochaEXPActor::AOmochaEXPActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetSphereRadius(CollectionDistance);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	OrbMesh->SetupAttachment(CollisionSphere);
	OrbMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CurrentAngle = FMath::RandRange(0.0f, 360.0f);
}

void AOmochaEXPActor::BeginPlay()
{
	Super::BeginPlay();

	// Overlap event bind
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AOmochaEXPActor::OnOverlapBegin);

	SetLifeSpan(LifeSpan);

	if (UWorld* World = GetWorld())
	{
		CurrentLevelName = World->GetMapName();
	}
}

void AOmochaEXPActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPlayer && IsValid(TargetPlayer))
	{
		MoveTowardsTarget(DeltaTime);

		static int32 FrameCounter = 0;
		if (++FrameCounter % 60 == 0)
		{
			if (UWorld* World = GetWorld())
			{
				FString NewLevelName = World->GetMapName();
				if (NewLevelName != CurrentLevelName)
				{
					Destroy();
					return;
				}
			}
		}
	}
	else
	{
		Destroy();
	}
}

void AOmochaEXPActor::InitializeXPOrb(AActor* InTargetPlayer)
{
	TargetPlayer = InTargetPlayer;
}

void AOmochaEXPActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (OtherActor == TargetPlayer)
	{
		if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(OtherActor))
		{
			// Todo: Add Effect, Sound
			Destroy();
		}
	}
}

void AOmochaEXPActor::MoveTowardsTarget(float DeltaTime)
{
	if (!TargetPlayer)
	{
		return;
	}

	FVector TargetLocation = TargetPlayer->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	FVector ToTarget = TargetLocation - CurrentLocation;
	float DistanceToTarget = ToTarget.Size();

	if (DistanceToTarget <= CollectionDistance)
	{
		Destroy();
		return;
	}

	if (DistanceToTarget > CollectionDistance)
	{
		// spiral rotation
		CurrentAngle += CircleSpeed * DeltaTime;
		if (CurrentAngle >= 360.0f)
		{
			CurrentAngle -= 360.0f;
		}

		// Target Direction Vectors
		FVector DirectionToTarget = ToTarget.GetSafeNormal();
		FVector RightVector = FVector::CrossProduct(DirectionToTarget, FVector::UpVector).GetSafeNormal();
		FVector UpVector = FVector::CrossProduct(RightVector, DirectionToTarget).GetSafeNormal();

		// Radian Movement Calcaultion
		float RadianAngle = FMath::DegreesToRadians(CurrentAngle);
		FVector CircleOffset = RightVector * FMath::Sin(RadianAngle) * CircleRadius;
		CircleOffset += UpVector * FMath::Cos(RadianAngle) * CircleRadius * 0.3f;

		// Target Default Movement + Radian Movement
		FVector DesiredDirection = DirectionToTarget + (CircleOffset.GetSafeNormal() * 0.3f);
		DesiredDirection.Normalize();

		FVector NewLocation = CurrentLocation + DesiredDirection * HomingSpeed * DeltaTime;
		SetActorLocation(NewLocation);

		SetActorRotation(DesiredDirection.Rotation());
	}

	else
	{
		FVector Direction = ToTarget.GetSafeNormal();
		float AcceleratedSpeed = HomingSpeed * 1.8f;

		FVector NewLocation = CurrentLocation + Direction * AcceleratedSpeed * DeltaTime;
		SetActorLocation(NewLocation);

		SetActorRotation(Direction.Rotation());
	}
}
