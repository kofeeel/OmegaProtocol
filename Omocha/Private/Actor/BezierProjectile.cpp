#include "Actor/BezierProjectile.h"

#include "Component/BezierMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

ABezierProjectile::ABezierProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Remove default ProjectileMovementComponent and replace with BezierMovementComponent
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->DestroyComponent();
        ProjectileMovementComponent = nullptr;
    }
    
    // Create bezier movement component
    BezierMovementComponent = CreateDefaultSubobject<UBezierMovementComponent>(TEXT("BezierMovementComponent"));
    
    // Setup bezier movement properties
    BezierMovementComponent->UpdatedComponent = RootComponent;
    BezierMovementComponent->ProjectileGravityScale = 0.0f; // Start with no gravity
    BezierMovementComponent->bUseBezierPath = bUseBezierByDefault;
    BezierMovementComponent->FlightDuration = DefaultFlightDuration;
    BezierMovementComponent->bRotateToMovementDirection = true;
    
    // Network replication
    bReplicates = true;
}

void ABezierProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    // Store initial position as trajectory start if not set
    if (TrajectoryStart.IsZero())
    {
        TrajectoryStart = GetActorLocation();
    }
}

void ABezierProjectile::SetBezierTrajectory(const FVector& StartPoint, const FVector& ControlPoint, const FVector& EndPoint, float FlightTime)
{
    if (!BezierMovementComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("BezierMovementComponent is null in SetBezierTrajectory"));
        return;
    }
    
    // Store trajectory points
    TrajectoryStart = StartPoint;
    TrajectoryControl = ControlPoint;
    TrajectoryEnd = EndPoint;
    
    // Set projectile position to start point
    SetActorLocation(StartPoint);
    
    // Configure bezier path
    BezierMovementComponent->SetBezierPath(StartPoint, ControlPoint, EndPoint, FlightTime);
}

void ABezierProjectile::StartBezierFlight()
{
    if (!BezierMovementComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("BezierMovementComponent is null in StartBezierFlight"));
        return;
    }
    
    // Enable bezier movement
    BezierMovementComponent->EnableBezierMovement(true);
}

float ABezierProjectile::GetFlightProgress() const
{
    if (!BezierMovementComponent)
    {
        return 0.0f;
    }
    
    return BezierMovementComponent->GetBezierProgress();
}