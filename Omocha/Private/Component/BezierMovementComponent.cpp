#include "Component/BezierMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

UBezierMovementComponent::UBezierMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    bUseBezierPath = false;
    FlightDuration = 2.0f;
    bRotateToMovementDirection = true;
    CurrentTime = 0.0f;
    bBezierCompleted = false;
    
    ProjectileGravityScale = 1.0f;
}

void UBezierMovementComponent::SetBezierPath(const FVector& P0, const FVector& P1, const FVector& P2, float Duration)
{
    BezierP0 = P0;
    BezierP1 = P1; 
    BezierP2 = P2;
    FlightDuration = FMath::Max(Duration, 0.1f); // Minimum duration
    
    CurrentTime = 0.0f;
    bBezierCompleted = false;
    
    if (UpdatedComponent)
    {
        PreviousLocation = UpdatedComponent->GetComponentLocation();
    }
}

void UBezierMovementComponent::EnableBezierMovement(bool bEnable)
{
    bUseBezierPath = bEnable;
    
    if (bEnable)
    {
        ProjectileGravityScale = 0.0f;
        bShouldBounce = false;
    }
    else
    {
        ProjectileGravityScale = 1.0f;
    }
}

void UBezierMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!bUseBezierPath || bBezierCompleted)
    {
        Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
        return;
    }

    if (!bUseBezierPath || bBezierCompleted || BezierP0.IsZero() && BezierP1.IsZero() && BezierP2.IsZero())
    {
        Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
        return;
    }
    
    if (!UpdatedComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdatedComponent is null in UBezierMovementComponent::TickComponent"));
        return;
    }
    
    CurrentTime += DeltaTime;
    float T = CurrentTime / FlightDuration;
    
    if (T >= 1.0f)
    {
        T = 1.0f;
        bBezierCompleted = true;
    }
    
    FVector NextPosition = CalculateBezierPoint(T);
    FVector CurrentLocation = UpdatedComponent->GetComponentLocation();

    Velocity = (NextPosition - CurrentLocation) / DeltaTime;

    UpdateComponentVelocity();
    
    if (bRotateToMovementDirection && !Velocity.IsNearlyZero())
    {
        FRotator NewRotation = Velocity.Rotation();
        UpdatedComponent->SetWorldRotation(NewRotation);
    }
    
    PreviousLocation = CurrentLocation;
    
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bBezierCompleted && T >= 1.0f)
    {
        EnableBezierMovement(false);
        
        FVector FinalDirection = (BezierP2 - PreviousLocation).GetSafeNormal();
        float FinalSpeed = Velocity.Size();
        Velocity = FinalDirection * FinalSpeed;
    }
}

FVector UBezierMovementComponent::CalculateBezierPoint(float T) const
{
    // curve P(t) = (1-t)²P0 + 2(1-t)tP1 + t²P2
    float OneMinusT = 1.0f - T;
    float OneMinusTSquared = OneMinusT * OneMinusT;
    float TSquared = T * T;
    float TwoOneMinusTT = 2.0f * OneMinusT * T;
    
    return (OneMinusTSquared * BezierP0) + (TwoOneMinusTT * BezierP1) + (TSquared * BezierP2);
}

FVector UBezierMovementComponent::CalculateBezierTangent(float T) const
{
    // P'(t) = 2(1-t)(P1-P0) + 2t(P2-P1)
    float OneMinusT = 1.0f - T;
    return 2.0f * OneMinusT * (BezierP1 - BezierP0) + 2.0f * T * (BezierP2 - BezierP1);
}
