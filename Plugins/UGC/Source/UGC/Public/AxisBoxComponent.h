// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AxisBaseComponent.h"
#include "AxisBoxComponent.generated.h"

/**
 * Simple Component intended to be used as part of 3D Gizmos.
 * Currently draws the "arrow" as a single 3D line.
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class UGC_API UAxisBoxComponent : public UAxisBaseComponent
{
    GENERATED_BODY()

public:
    UAxisBoxComponent();

public:
    UPROPERTY(EditAnywhere, Category = Options)
    float Width = 200.0f;
    UPROPERTY(EditAnywhere, Category = Options)
    float Height = 200.0f;
    UPROPERTY(EditAnywhere, Category = Options)
    float Length = 200.0f;
    UPROPERTY(EditAnywhere, Category = Options)
    float Thickness = 2.0f;
private:
    //~ Begin UPrimitiveComponent Interface.
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    virtual bool LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params) override;
    //~ End UPrimitiveComponent Interface.

    //~ Begin USceneComponent Interface.
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    //~ Begin USceneComponent Interface.
};
