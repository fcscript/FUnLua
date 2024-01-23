#include "AxisBoxComponent.h"

#include "AxisGizmoRenderingUtil.h"

class FUGCGizmoBoxComponentSceneProxy final : public FPrimitiveSceneProxy
{
public:
    SIZE_T GetTypeHash() const override
    {
        static size_t UniquePointer;
        return reinterpret_cast<size_t>(&UniquePointer);
    }

    FUGCGizmoBoxComponentSceneProxy(UAxisBoxComponent* InComponent)
        : FPrimitiveSceneProxy(InComponent)
        , AxisCompnent(InComponent)
    {
    }

    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
    {
        bool bRenderVisibility = AxisCompnent->bRenderVisibility;
        if (!bRenderVisibility)
        {
            return;
        }

        const FLinearColor& Color = AxisCompnent->Color;
        const float Width = AxisCompnent->Width;
        const float Height = AxisCompnent->Height;
        const float Length = AxisCompnent->Length;
        const float Thickness = AxisCompnent->Thickness;
        const float HoverThicknessMultiplier = AxisCompnent->HoverSizeMultiplier;

        bool& bExternalRenderVisibility = AxisCompnent->bExternalRenderVisibility;
        float RY = Width * 0.5f;
        float RZ = Height * 0.5f;
        float RX = Length * 0.5f;

        FVector  V[8] = 
        {
            FVector(-RX,  RY, -RZ),   // v0
            FVector(-RX, -RY, -RZ),   // v1
            FVector( RX, -RY, -RZ),   // v2
            FVector( RX,  RY, -RZ),   // v3

            FVector(-RX,  RY,  RZ),   // v4
            FVector(-RX, -RY,  RZ),   // v5
            FVector( RX, -RY,  RZ),   // v6
            FVector( RX,  RY,  RZ),   // v7
        };
        int  Indics[24] = {0, 1, 0, 3, 1, 2, 2, 3, 2, 6, 6, 7, 3, 7, 1, 5, 5, 4, 0, 4, 4, 7, 5, 6 };
        //      v4------v7
        //     /        /|
        //    v0------v3 |
        //    |       |  |       Z
        //    | v5    | v6       |  /  y
        //    |       |/         | / 
        //    v1------v2         0 ---- x

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

                for (int i = 0; i < 24;)
                {
                    FVector  V1 = V[Indics[i++]];
                    FVector  V2 = V[Indics[i++]];
                    V1 = LocalToWorldMatrix.TransformPosition(V1);
                    V2 = LocalToWorldMatrix.TransformPosition(V2);

                    PDI->DrawLine(V1, V2, Color, SDPG_Foreground, Thickness, 0.0f, true);
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
        Result.bNormalTranslucency = true;
        return Result;
    }

    virtual bool CanBeOccluded() const override
    {
        return false;
    }

    virtual uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
    uint32 GetAllocatedSize(void) const { return FPrimitiveSceneProxy::GetAllocatedSize(); }
public:
    UAxisBoxComponent* AxisCompnent = nullptr;
};

UAxisBoxComponent::UAxisBoxComponent()
{
    
}


FPrimitiveSceneProxy* UAxisBoxComponent::CreateSceneProxy()
{
    FUGCGizmoBoxComponentSceneProxy* NewProxy = new FUGCGizmoBoxComponentSceneProxy(this);

    return NewProxy;
}

bool UAxisBoxComponent::LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params)
{
    if (bRenderVisibility == false)
    {
        return false;
    }

    return true;
}

FBoxSphereBounds UAxisBoxComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    float Radius = Width < Height ? Height :Width;
    Radius = Radius < Length ? Length : Radius;
    return FBoxSphereBounds(FSphere(FVector::ZeroVector, Radius).TransformBy(LocalToWorld));
}
