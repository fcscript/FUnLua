// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AxisBaseComponent.h"
#include "AxisTranstromComponent.generated.h"

/**
 * Simple Component intended to be used as part of 3D Gizmos.
 * Currently draws the "arrow" as a single 3D line.
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class UGC_API UAxisTranstromComponent : public UAxisBaseComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Options)
	FVector Direction = FVector(0,0,1);

	UPROPERTY(EditAnywhere, Category = Options)
	float Gap = 5.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	float Length = 60.0f;

	UPROPERTY(EditAnywhere, Category = Options)
	float Thickness = 2.0f;

    // if true, we drew along -Direction instead of Direction, and so should hit-test accordingly
    UPROPERTY(EditAnywhere, Category = Options)
    bool bFlipped;

    UPROPERTY(EditAnywhere, Category = Options)
    FVector   RenderStartPos;
    UPROPERTY(EditAnywhere, Category = Options)
    FVector   RenderEndPos;
private:
	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params) override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.
};
