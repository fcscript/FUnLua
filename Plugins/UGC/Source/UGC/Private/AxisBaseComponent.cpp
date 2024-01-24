// Copyright Epic Games, Inc. All Rights Reserved.
 
#include "AxisBaseComponent.h"
#include "Kismet/GameplayStatics.h"

bool UAxisBaseComponent::IsPick(APlayerController const* Player, const FVector2D& ScreenPos, float fDist)
{
    if (!bRenderVisibility || !bExternalRenderVisibility)
    {
        return false;
    }
    FVector WorldPosition;
    FVector WorldDirection;
    UGameplayStatics::DeprojectScreenToWorld(Player, ScreenPos, WorldPosition, WorldDirection);

    FVector TraceStart = WorldPosition;
    FVector TraceEnd = TraceStart + WorldDirection * 100000;

    FVector HitLocation;
    FVector HitNormal;
    FName  OutBoneName;
    FHitResult  OutHit;
    bool bPick = K2_LineTraceComponent(TraceStart, TraceEnd, false, false, false, HitLocation, HitNormal, OutBoneName, OutHit);
    return bPick;
}