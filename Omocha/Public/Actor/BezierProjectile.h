#pragma once

#include "CoreMinimal.h"
#include "OmochaGrenade.h"
#include "Actor/OmochaProjectile.h"
#include "BezierProjectile.generated.h"

class UBezierMovementComponent;

UCLASS()
class OMOCHA_API ABezierProjectile : public AOmochaGrenade
{
	GENERATED_BODY()

public:
	ABezierProjectile();

	// Set bezier path for missile trajectory
	UFUNCTION(BlueprintCallable, Category = "Missile")
	void SetBezierTrajectory(const FVector& StartPoint, const FVector& ControlPoint, const FVector& EndPoint, float FlightTime = 2.0f);
    
	// Start bezier movement
	UFUNCTION(BlueprintCallable, Category = "Missile")
	void StartBezierFlight();
    
	// Get flight progress (0-1)
	UFUNCTION(BlueprintPure, Category = "Missile")
	float GetFlightProgress() const;

protected:
	virtual void BeginPlay() override;

	// Bezier movement component (replaces default ProjectileMovement)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UBezierMovementComponent> BezierMovementComponent;
    
	// Flight duration for bezier trajectory
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile")
	float DefaultFlightDuration = 2.0f;
    
	// Whether to use bezier movement by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile")
	bool bUseBezierByDefault = true;

private:
	// Store trajectory points for debugging
	FVector TrajectoryStart = FVector::ZeroVector;
	FVector TrajectoryControl = FVector::ZeroVector;
	FVector TrajectoryEnd = FVector::ZeroVector;
};