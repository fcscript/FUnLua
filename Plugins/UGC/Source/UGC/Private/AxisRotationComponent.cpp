#include "AxisRotationComponent.h"

//#include "BaseGizmos/GizmoMath.h"
//#include "BaseGizmos/GizmoRenderingUtil.h"

#include "AxisGizmoRenderingUtil.h"

class FUGCGizmoCircleComponentSceneProxy final : public FPrimitiveSceneProxy
{
public:
    SIZE_T GetTypeHash() const override
    {
        static size_t UniquePointer;
        return reinterpret_cast<size_t>(&UniquePointer);
    }

    FUGCGizmoCircleComponentSceneProxy(UAxisRotationComponent* InComponent)
        : FPrimitiveSceneProxy(InComponent),
        AxisCompnent(InComponent)
    {
    }

    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
    {
        if(!AxisCompnent->bRenderVisibility)
        {
            return ;
        }
        const FLinearColor& Color = AxisCompnent->Color;
        const FVector& Normal = AxisCompnent->Normal;
        const float Radius = AxisCompnent->Radius;
        const float Thickness = AxisCompnent->Thickness;
        const int NumSides = AxisCompnent->NumSides;
        const bool bViewAligned = AxisCompnent->bViewAligned;
        const float HoverThicknessMultiplier = AxisCompnent->HoverSizeMultiplier;

        // try to find focused scene view. May return nullptr.
        const FSceneView* FocusedView = AxisGizmoRenderingUtil::FindFocusedEditorSceneView(Views, ViewFamily, VisibilityMap);

        const FMatrix& LocalToWorldMatrix = GetLocalToWorld();
        FVector Origin = LocalToWorldMatrix.TransformPosition(FVector::ZeroVector);
        FVector PlaneX, PlaneY;
        AxisGizmoRenderingUtil::MakeNormalPlaneBasis(Normal, PlaneX, PlaneY);

        //FMaterialRenderProxy* MaterialProxy = nullptr;
        //MaterialProxy = Material->GetRenderProxy();
        //if (!MaterialProxy) return;

        float &DynamicPixelToWorldScale = AxisCompnent->DynamicPixelToWorldScale;
        bool &bExternalRenderVisibility = AxisCompnent->bExternalRenderVisibility;
        bool &bCircleIsViewPlaneParallel = AxisCompnent->bCircleIsViewPlaneParallel;
        bool bHovering = AxisCompnent->bHovering;
        bool bWorld = AxisCompnent->bWorld;
        UMaterialInterface* Material = AxisCompnent->Material;

        bExternalRenderVisibility = true;
        bCircleIsViewPlaneParallel = false;

        for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
        {
            if (VisibilityMap & (1 << ViewIndex))
            {
                const FSceneView* View = Views[ViewIndex];

                int32 LineIdx = 0;
                //FDynamicMeshBuilder MeshBuilder(View->GetFeatureLevel());
                FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
                bool bIsFocusedView = (FocusedView != nullptr && View == FocusedView);
                bool bIsOrtho = !View->IsPerspectiveProjection();
                FVector UpVector = View->GetViewUp();
                FVector ViewVector = View->GetViewDirection();

                // direction to origin of gizmo
                FVector GizmoViewDirection =
                    (bIsOrtho) ? (View->GetViewDirection()) : (Origin - View->ViewLocation);
                GizmoViewDirection.Normalize();

                float PixelToWorldScale = AxisGizmoRenderingUtil::CalculateLocalPixelToWorldScale(View, Origin);
                float LengthScale = PixelToWorldScale;
                DynamicPixelToWorldScale = PixelToWorldScale;

                double UseRadius = LengthScale * Radius;

                FLinearColor BackColor = FLinearColor(0.5f, 0.5f, 0.5f);
                float BackThickness = 0.5f;

                float UseThickness = bHovering ? (HoverThicknessMultiplier * Thickness) : (Thickness);
                if (!bIsOrtho)
                {
                    UseThickness *= (View->FOV / 90.0);		// compensate for FOV scaling in Gizmos...
                    BackThickness *= (View->FOV / 90.0);		// compensate for FOV scaling in Gizmos...
                }

                const float	AngleDelta = 2.0f * PI / NumSides;

                if (bViewAligned)
                {
                    FVector WorldOrigin = LocalToWorldMatrix.TransformPosition(FVector::ZeroVector);
                    WorldOrigin += 0.001 * ViewVector;
                    FVector WorldPlaneX, WorldPlaneY;
                    AxisGizmoRenderingUtil::MakeNormalPlaneBasis(ViewVector, WorldPlaneX, WorldPlaneY);

                    FVector	LastVertex = WorldOrigin + WorldPlaneX * UseRadius;
                    for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
                    {
                        float DeltaX = FMath::Cos(AngleDelta * (SideIndex + 1));
                        float DeltaY = FMath::Sin(AngleDelta * (SideIndex + 1));
                        const FVector DeltaVector = WorldPlaneX * DeltaX + WorldPlaneY * DeltaY;
                        const FVector Vertex = WorldOrigin + UseRadius * DeltaVector;
                        PDI->DrawLine(LastVertex, Vertex, Color, SDPG_Foreground, UseThickness, 0.0f, true);
                        //AAuroraTransformGizmoActor::DrawLine_RenderThread(View, MeshBuilder, LineIdx++, LastVertex, Vertex, UseThickness, Color);

                        LastVertex = Vertex;
                    }
                }
                else
                {
                    FVector WorldOrigin = LocalToWorldMatrix.TransformPosition(FVector::ZeroVector);
                    bool bWorldAxis = bWorld;
                    FVector WorldPlaneX = (bWorldAxis) ? PlaneX : FVector{ LocalToWorldMatrix.TransformVector(PlaneX) };
                    FVector WorldPlaneY = (bWorldAxis) ? PlaneY : FVector{ LocalToWorldMatrix.TransformVector(PlaneY) };

                    FVector PlaneWorldNormal = (bWorldAxis) ? Normal : FVector{ LocalToWorldMatrix.TransformVector(Normal) };
                    double ViewDot = FVector::DotProduct(GizmoViewDirection, PlaneWorldNormal);
                    bool bOnEdge = FMath::Abs(ViewDot) < 0.05;
                    bool bIsViewPlaneParallel = FMath::Abs(ViewDot) > 0.95;
                    if (bIsFocusedView)
                    {
                        bCircleIsViewPlaneParallel = bIsViewPlaneParallel;
                    }

                    bool bRenderVisibility = !bOnEdge;
                    bExternalRenderVisibility = bRenderVisibility;
                    if (bRenderVisibility)
                    {
                        if (bIsViewPlaneParallel)
                        {
                            FVector	LastVertex = WorldOrigin + WorldPlaneX * UseRadius;
                            for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
                            {
                                float DeltaX = FMath::Cos(AngleDelta * (SideIndex + 1));
                                float DeltaY = FMath::Sin(AngleDelta * (SideIndex + 1));
                                const FVector DeltaVector = WorldPlaneX * DeltaX + WorldPlaneY * DeltaY;
                                const FVector Vertex = WorldOrigin + UseRadius * DeltaVector;
                                PDI->DrawLine(LastVertex, Vertex, Color, SDPG_Foreground, UseThickness, 0.0f, true);
                                //AAuroraTransformGizmoActor::DrawLine_RenderThread(View, MeshBuilder, LineIdx++, LastVertex, Vertex, UseThickness, Color);
                                LastVertex = Vertex;
                            }
                        }
                        else
                        {
                            FVector	LastVertex = WorldOrigin + WorldPlaneX * UseRadius;
                            bool bLastVisible = FVector::DotProduct(WorldPlaneX, GizmoViewDirection) < 0;
                            for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
                            {
                                float DeltaX = FMath::Cos(AngleDelta * (SideIndex + 1));
                                float DeltaY = FMath::Sin(AngleDelta * (SideIndex + 1));
                                const FVector DeltaVector = WorldPlaneX * DeltaX + WorldPlaneY * DeltaY;
                                const FVector Vertex = WorldOrigin + UseRadius * DeltaVector;
                                bool bVertexVisible = FVector::DotProduct(DeltaVector, GizmoViewDirection) < 0;
                                if (bLastVisible && bVertexVisible)
                                {
                                    PDI->DrawLine(LastVertex, Vertex, Color, SDPG_Foreground, UseThickness, 0.0f, true);
                                    //AAuroraTransformGizmoActor::DrawLine_RenderThread(View, MeshBuilder, LineIdx++, LastVertex, Vertex, UseThickness, Color);
                                }
                                else
                                {
                                    PDI->DrawLine(LastVertex, Vertex, BackColor, SDPG_Foreground, BackThickness, 0.0f, true);
                                    //AAuroraTransformGizmoActor::DrawLine_RenderThread(View, MeshBuilder, LineIdx++, LastVertex, Vertex, UseThickness, Color);
                                }
                                bLastVisible = bVertexVisible;
                                LastVertex = Vertex;
                            }
                        }
                    }
                }
                //MeshBuilder.GetMesh(FMatrix::Identity, MaterialProxy, SDPG_Foreground, false, false, ViewIndex, Collector);
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
    // set on Component for use in ::GetDynamicMeshElements()
    UAxisRotationComponent* AxisCompnent = nullptr;
};


UAxisRotationComponent::UAxisRotationComponent()
{

}

FPrimitiveSceneProxy* UAxisRotationComponent::CreateSceneProxy()
{
    FUGCGizmoCircleComponentSceneProxy* NewProxy = new FUGCGizmoCircleComponentSceneProxy(this);
    return NewProxy;
}

bool UAxisRotationComponent::LineTraceComponent(FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params)
{
    if (!bRenderVisibility || !bExternalRenderVisibility)
    {
        return false;
    }

    float LengthScale = DynamicPixelToWorldScale;
    double UseRadius = LengthScale * Radius;

    const FTransform& Transform = this->GetComponentToWorld();
    FVector WorldNormal = (bWorld) ? Normal : Transform.TransformVector(Normal);
    FVector WorldOrigin = Transform.TransformPosition(FVector::ZeroVector);

    FRay Ray(Start, End - Start, false);

    // Find the intresection with the circle plane. Note that unlike the FMath version, AxisGizmoRenderingUtil::RayPlaneIntersectionPoint() 
    // checks that the ray isn't parallel to the plane.
    bool bIntersects;
    FVector HitPos;
    AxisGizmoRenderingUtil::RayPlaneIntersectionPoint(WorldOrigin, WorldNormal, Ray.Origin, Ray.Direction, bIntersects, HitPos);
    if (!bIntersects || Ray.GetParameter(HitPos) > Ray.GetParameter(End))
    {
        return false;
    }

    FVector NearestCircle;
    AxisGizmoRenderingUtil::ClosetPointOnCircle(HitPos, WorldOrigin, WorldNormal, UseRadius, NearestCircle);

    FVector NearestRay = Ray.ClosestPoint(NearestCircle);

    double Distance = FVector::Distance(NearestCircle, NearestRay);
    if (Distance > PixelHitDistanceThreshold * DynamicPixelToWorldScale)
    {
        return false;
    }

    // filter out hits on "back" of sphere that circle lies on
    if (bOnlyAllowFrontFacingHits && bCircleIsViewPlaneParallel == false)
    {
        bool bSphereIntersects = false;
        FVector SphereHitPoint;
        FVector RayToCirclePointDirection = (NearestCircle - Ray.Origin);
        RayToCirclePointDirection.Normalize();
        AxisGizmoRenderingUtil::RaySphereIntersection(
            WorldOrigin, UseRadius, Ray.Origin, RayToCirclePointDirection, bSphereIntersects, SphereHitPoint);
        if (bSphereIntersects)
        {
            if (FVector::DistSquared(SphereHitPoint, NearestCircle) > UseRadius * 0.1f)
            {
                return false;
            }
        }
    }

    OutHit.Component = this;
    OutHit.Distance = FVector::Distance(Start, NearestRay);
    OutHit.ImpactPoint = NearestRay;
    return true;
}

FBoxSphereBounds UAxisRotationComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    return FBoxSphereBounds(FSphere(FVector::ZeroVector, Radius).TransformBy(LocalToWorld));    
}
