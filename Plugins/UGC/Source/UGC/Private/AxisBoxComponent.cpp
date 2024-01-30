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
        const float Thickness = AxisCompnent->Thickness;
        const float HoverThicknessMultiplier = AxisCompnent->HoverSizeMultiplier;

        bool& bExternalRenderVisibility = AxisCompnent->bExternalRenderVisibility;
        const FSceneView* FocusedView = AxisGizmoRenderingUtil::FindFocusedEditorSceneView(Views, ViewFamily, VisibilityMap);
        bool& bExternalHoverState = AxisCompnent->bHovering;

        //const FMatrix& LocalToWorldMatrix = GetLocalToWorld();
        //FVector Origin = LocalToWorldMatrix.TransformPosition(FVector::ZeroVector);

        bExternalRenderVisibility = true;
        ESceneDepthPriorityGroup DepthType = AxisCompnent->SceneDepthGroup;

        for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
        {
            if (VisibilityMap & (1 << ViewIndex))
            {
                const FSceneView* View = Views[ViewIndex];
                FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
                for(int32 i = 0; i< AxisCompnent->Boxs.Num(); ++i)
                {
                    DrawBox(PDI, AxisCompnent->Boxs[i], Color, DepthType, Thickness);
                }

                if(AxisCompnent->Boxs.Num() > 1 && AxisCompnent->bShowBigBox)
                {
                    DrawBox(PDI, AxisCompnent->BigBox, Color, DepthType, Thickness);
                }
            }
        }
    }
    void  DrawBox(FPrimitiveDrawInterface* PDI, const FBox &Box, const FLinearColor& Color, ESceneDepthPriorityGroup DepthType, float Thickness) const
    {
        FVector Center = Box.GetCenter();
        FVector Extent = Box.GetExtent();
        float RY = Extent.Y;
        float RZ = Extent.Z;
        float RX = Extent.X;

        FVector  V[8] =
        {
            FVector(-RX,  RY, -RZ),   // v0
            FVector(-RX, -RY, -RZ),   // v1
            FVector(RX, -RY, -RZ),   // v2
            FVector(RX,  RY, -RZ),   // v3

            FVector(-RX,  RY,  RZ),   // v4
            FVector(-RX, -RY,  RZ),   // v5
            FVector(RX, -RY,  RZ),   // v6
            FVector(RX,  RY,  RZ),   // v7
        };
        const int  Indics[24] = { 0, 1, 0, 3, 1, 2, 2, 3, 2, 6, 6, 7, 3, 7, 1, 5, 5, 4, 0, 4, 4, 7, 5, 6 };
        //      v4------v7
        //     /        /|
        //    v0------v3 |
        //    |       |  |       Z
        //    | v5    | v6       |  /  y
        //    |       |/         | / 
        //    v1------v2         0 ---- x

        for (int i = 0; i < 24;)
        {
            FVector  V1 = V[Indics[i++]];
            FVector  V2 = V[Indics[i++]];
            V1 += Center;
            V2 += Center;

            PDI->DrawLine(V1, V2, Color, DepthType, Thickness, 0.0f, true);
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

void UAxisBoxComponent::UpdateBounds()
{
    if(Boxs.Num() > 0)
    {
        BigBox = Boxs[0];
    }
    else
    {
        BigBox = FBox();
    }
    for(int32 i = 1; i<Boxs.Num(); ++i)
    {
        const FBox &Box = Boxs[i];
        BigBox += Box;
    }
}

FBoxSphereBounds UAxisBoxComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    //return FBoxSphereBounds(BigBox.TransformBy(LocalToWorld));
    FVector Size = BigBox.GetSize();
    float Radius = FMath::Max3(Size.X, Size.Y, Size.Z);
    Radius = FMath::Max(Radius, 1.0f);
    Radius *= 2.0f;
    //return FBoxSphereBounds(FSphere(FVector::ZeroVector, Radius).TransformBy(LocalToWorld));
    return FBoxSphereBounds(BigBox);
}
