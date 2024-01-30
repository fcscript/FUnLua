// Copyright Epic Games, Inc. All Rights Reserved.
 
#include "AxisTranstromComponent.h"
#include "PrimitiveSceneProxy.h"
#include "Materials/Material.h"
#include "MaterialShared.h"
#include "SceneManagement.h"

#include "Kismet/GameplayStatics.h"
#include "AxisGizmoRenderingUtil.h"

class FAxisTranstromComponentSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FAxisTranstromComponentSceneProxy(UAxisTranstromComponent* InComponent)
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
        const FVector &Direction = AxisCompnent->Direction;
        const float Gap = AxisCompnent->Gap;
        const float Length = AxisCompnent->Length;
        const float Thickness = AxisCompnent->Thickness;
        const float HoverThicknessMultiplier = AxisCompnent->HoverSizeMultiplier;

        bool &bExternalRenderVisibility = AxisCompnent->bExternalRenderVisibility;
        bool &bExternalHoverState = AxisCompnent->bHovering;
        bool &bExternalWorldLocalState = AxisCompnent->bWorld;
        bool &bFlippedExternal = AxisCompnent->bFlipped;
        float &ExternalDynamicPixelToWorldScale = AxisCompnent->DynamicPixelToWorldScale;
		// try to find focused scene view. May return nullptr.
		const FSceneView* FocusedView = AxisGizmoRenderingUtil::FindFocusedEditorSceneView(Views, ViewFamily, VisibilityMap);

		const FMatrix& LocalToWorldMatrix = GetLocalToWorld();
		FVector Origin = LocalToWorldMatrix.TransformPosition(FVector::ZeroVector);

        bExternalRenderVisibility = true;

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
				bool bIsFocusedView = (FocusedView != nullptr && View == FocusedView);
				bool bIsOrtho = !View->IsPerspectiveProjection();

				// direction to origin of gizmo
				FVector ViewDirection = 
					(bIsOrtho) ?  (View->GetViewDirection()) : (Origin - View->ViewLocation);
				ViewDirection.Normalize();

				bool bWorldAxis = bExternalWorldLocalState;
				FVector ArrowDirection = (bWorldAxis) ? Direction : FVector{ LocalToWorldMatrix.TransformVector(Direction) };
				bool bFlipped = (FVector::DotProduct(ViewDirection, ArrowDirection) > 0);
				ArrowDirection = (bFlipped) ? -ArrowDirection : ArrowDirection;
				if (bIsFocusedView)
				{
					bFlippedExternal = bFlipped;
				}

				//bRenderVisibility = FMath::Abs(FVector::DotProduct(ArrowDirection, ViewDirection)) < 0.985f;   // ~10 degrees
				bRenderVisibility = FMath::Abs(FVector::DotProduct(ArrowDirection, ViewDirection)) < 0.965f;   // ~15 degrees

				if (bIsFocusedView)
				{
                    bExternalRenderVisibility = bRenderVisibility;
				}
				if (!bRenderVisibility)
				{
					continue;
				}

				float PixelToWorldScale = AxisGizmoRenderingUtil::CalculateLocalPixelToWorldScale(View, Origin);
				float LengthScale = PixelToWorldScale;
				if (bIsFocusedView)
				{
					ExternalDynamicPixelToWorldScale = PixelToWorldScale;
				}

				float UseThickness = bExternalHoverState ? (HoverThicknessMultiplier * Thickness) : (Thickness);
				if (!bIsOrtho)
				{
					UseThickness *= (View->FOV / 90.0);		// compensate for FOV scaling in Gizmos...
				}

				double StartDist = LengthScale * Gap;
				double EndDist = LengthScale * (Gap + Length);

				FVector StartPoint = Origin + StartDist*ArrowDirection;
				FVector EndPoint = Origin + EndDist*ArrowDirection;

                AxisCompnent->RenderStartPos = StartPoint;
                AxisCompnent->RenderEndPos = EndPoint;

				PDI->DrawLine(StartPoint, EndPoint, Color, AxisCompnent->SceneDepthGroup, UseThickness, 0.0f, true);
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
    UAxisTranstromComponent* AxisCompnent = nullptr;
};


FPrimitiveSceneProxy* UAxisTranstromComponent::CreateSceneProxy()
{
	FAxisTranstromComponentSceneProxy* NewProxy = new FAxisTranstromComponentSceneProxy(this);
	return NewProxy;
}

FBoxSphereBounds UAxisTranstromComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FSphere(FVector::ZeroVector, Gap+Length).TransformBy(LocalToWorld));
}

bool UAxisTranstromComponent::LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params)
{
	if (!bRenderVisibility || !bExternalRenderVisibility)
	{
		return false;
	}

	const FTransform& Transform = this->GetComponentToWorld();

	FVector UseDirection = (bFlipped) ? -Direction : Direction;
	float LengthScale = DynamicPixelToWorldScale;
	double StartDist = LengthScale * Gap;
	double EndDist = LengthScale * (Gap + Length);

	UseDirection = (bWorld) ? UseDirection : Transform.TransformVector(UseDirection);
	FVector UseOrigin = Transform.TransformPosition(FVector::ZeroVector);
	//FVector Point0 = UseOrigin + StartDist * UseDirection;
	//FVector Point1 = UseOrigin + EndDist * UseDirection;
    FVector Point0 = RenderStartPos;
    FVector Point1 = RenderEndPos;

	FVector NearestArrow, NearestLine;
	FMath::SegmentDistToSegmentSafe(Point0, Point1, Start, End, NearestArrow, NearestLine);
	double Distance = FVector::Distance(NearestArrow, NearestLine);
	if (Distance > PixelHitDistanceThreshold*DynamicPixelToWorldScale)
	{
		return false;
	}

	OutHit.Component = this;
	OutHit.Distance = FVector::Distance(Start, NearestLine);
	OutHit.ImpactPoint = NearestLine;
	return true;
}
