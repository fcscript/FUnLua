// Copyright Epic Games, Inc. All Rights Reserved.
 
#include "AxisDebugLineComponent.h"
#include "PrimitiveSceneProxy.h"
#include "Materials/Material.h"
#include "MaterialShared.h"
#include "SceneManagement.h"

#include "Kismet/GameplayStatics.h"
#include "AxisGizmoRenderingUtil.h"

class FAxisDebugLineComponentSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

    FAxisDebugLineComponentSceneProxy(UAxisDebugLineComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent),
        AxisCompnent(InComponent)
	{
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
        bool bRenderVisibility = AxisCompnent->bRenderVisibility;
        if(!bRenderVisibility)
        {
            return ;
        }
        const FLinearColor &Color = AxisCompnent->Color;
        const float Thickness = AxisCompnent->Thickness;
        const float HoverThicknessMultiplier = AxisCompnent->HoverSizeMultiplier;

		// try to find focused scene view. May return nullptr.
		//const FMatrix& LocalToWorldMatrix = GetLocalToWorld();
        //const FMatrix& WorldToLocalMatrix = LocalToWorldMatrix.Inverse();
        const TArray<FVector> &Points = AxisCompnent->Points;

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

                for(int32 i = 0; i + 1< AxisCompnent->Points.Num(); ++i)
                {
                    PDI->DrawLine(Points[i], Points[i+1], Color, AxisCompnent->SceneDepthGroup, Thickness, 0.0f, true);
                }
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = false;
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return false;
	}

	virtual uint32 GetMemoryFootprint(void) const override { return sizeof *this + GetAllocatedSize(); }
	uint32 GetAllocatedSize(void) const { return FPrimitiveSceneProxy::GetAllocatedSize(); }
public:
    UAxisDebugLineComponent* AxisCompnent = nullptr;
};


FPrimitiveSceneProxy* UAxisDebugLineComponent::CreateSceneProxy()
{
    FAxisDebugLineComponentSceneProxy* NewProxy = new FAxisDebugLineComponentSceneProxy(this);
	return NewProxy;
}

FBoxSphereBounds UAxisDebugLineComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(Points.GetData(), Points.Num()).TransformBy(LocalToWorld));
}

bool UAxisDebugLineComponent::LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params)
{
	if (!bRenderVisibility)
	{
		return false;
	}

	const FTransform& Transform = this->GetComponentToWorld();
	FVector NearestArrow, NearestLine;
    for (int32 i = 0; i + 1 < Points.Num(); ++i)
    {
        FMath::SegmentDistToSegmentSafe(Points[i], Points[i+1], Start, End, NearestArrow, NearestLine);
        double Distance = FVector::Distance(NearestArrow, NearestLine);
        if (Distance > PixelHitDistanceThreshold * DynamicPixelToWorldScale)
        {
            continue;
        }
        OutHit.Component = this;
        OutHit.Distance = FVector::Distance(Start, NearestLine);
        OutHit.ImpactPoint = NearestLine;
        return true;
    }
	return false;
}
