// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BezierMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UBezierMovementComponent : public UProjectileMovementComponent
{
    GENERATED_BODY()

public:
    UBezierMovementComponent();

    UFUNCTION(BlueprintCallable, Category = "Bezier Movement")
    void SetBezierPath(const FVector& P0, const FVector& P1, const FVector& P2, float Duration = 2.0f);
    
    UFUNCTION(BlueprintCallable, Category = "Bezier Movement")
    void EnableBezierMovement(bool bEnable);
    
    UFUNCTION(BlueprintPure, Category = "Bezier Movement")
    float GetBezierProgress() const { return CurrentTime / FlightDuration; }

protected:
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    FVector CalculateBezierPoint(float T) const;
    
    FVector CalculateBezierTangent(float T) const;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bezier Movement")
    bool bUseBezierPath = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bezier Movement")
    FVector BezierP0 = FVector::ZeroVector; // 시작점
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bezier Movement")  
    FVector BezierP1 = FVector::ZeroVector; // 제어점
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bezier Movement")
    FVector BezierP2 = FVector::ZeroVector; // 끝점
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bezier Movement", meta = (ClampMin = "0.1"))
    float FlightDuration = 2.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bezier Movement")
    bool bRotateToMovementDirection = true;

private:
    float CurrentTime = 0.0f;
    
    FVector PreviousLocation = FVector::ZeroVector;
    
    bool bBezierCompleted = false;
};