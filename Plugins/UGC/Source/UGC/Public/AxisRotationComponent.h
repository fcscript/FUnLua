// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AxisBaseComponent.h"
#include "AxisRotationComponent.generated.h"

/**
 * Simple Component intended to be used as part of 3D Gizmos.
 * Currently draws the "arrow" as a single 3D line.
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class UGC_API UAxisRotationComponent : public UAxisBaseComponent
{
    GENERATED_BODY()

public:
    UAxisRotationComponent();

public:
    UPROPERTY(EditAnywhere, Category = Options)
    FVector Normal = FVector(0, 0, 1);

    UPROPERTY(EditAnywhere, Category = Options)
    float Radius = 100.0f;

    UPROPERTY(EditAnywhere, Category = Options)
    float Thickness = 2.0f;

    UPROPERTY(EditAnywhere, Category = Options)
    int NumSides = 64;

    UPROPERTY(EditAnywhere, Category = Options)
    bool bViewAligned = false;

    // set to true if circle is fully visible, in which case we ignore bOnlyAllowFrontFacingHits
    UPROPERTY(EditAnywhere, Category = Options)
    bool bCircleIsViewPlaneParallel = false;

    UPROPERTY()
    UMaterialInterface* Material = nullptr;
    // If the circle was on a 3D sphere, then only the 'front' part of the circle can be hit, 
    // (in other words, if the ray would hit the sphere first, ignore the hit).
    // Dynamically disabled if the circle is parallel to the view plane (ie "fully visible")
    UPROPERTY(EditAnywhere, Category = Options)
    bool bOnlyAllowFrontFacingHits = true;
private:
    //~ Begin UPrimitiveComponent Interface.
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    virtual bool LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params) override;
    //~ End UPrimitiveComponent Interface.

    //~ Begin USceneComponent Interface.
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    //~ Begin USceneComponent Interface.
};
