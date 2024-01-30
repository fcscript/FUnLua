#include "UGCFunctionLibary.h"
#include "Engine/World.h"
#include "SceneView.h"
#include "Engine/LocalPlayer.h"
#include "Slate/SGameLayerManager.h"
#include "Math/UnrealMathUtility.h"


bool UUGCFunctionLibary::GetRenderTextureColors(TArray<FColor>& OutColors, UTextureRenderTarget2D* SourceRenderTarget)
{
    if(SourceRenderTarget)
    {
        FRenderTarget* RenderTarget = SourceRenderTarget->GameThread_GetRenderTargetResource();
        return RenderTarget->ReadPixels(OutColors);
    }
    return false;
}

FVector2D  UUGCFunctionLibary::GetLastMousePosition()
{
    FVector2D  MousePosition;
    if(GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetMousePosition(MousePosition);
    }
    return MousePosition;
}

void  UUGCFunctionLibary::SetActorSelectEffect(AActor* InActor)
{
    if (InActor)
    {
        TArray<UMeshComponent*> MeshComponents;
        InActor->GetComponents(MeshComponents);
        for (auto MeshComp : MeshComponents)
        {
            MeshComp->SetRenderCustomDepth(true);
            MeshComp->SetCustomDepthStencilValue(2);
            MeshComp->MarkRenderStateDirty();
            MeshComp->UpdateBounds();
        }
    }
}

APostProcessVolume* UUGCFunctionLibary::CreatePostProcessActor(const FPostProcessSettings &PostProcessSettings)
{
    if(GEngine && GEngine->GameViewport)
    {
        if(UWorld *World = GEngine->GameViewport->GetWorld())
        {
            APostProcessVolume *PostActor = Cast<APostProcessVolume>(World->SpawnActor(APostProcessVolume::StaticClass()));
            if(PostActor)
            {
                PostActor->Priority = 100000;
                //World->bPostProcessVolumePriorityDirty = true;
                PostActor->bUnbound = true;
                PostActor->Settings = PostProcessSettings;
                return PostActor;
            }
        }
    }
    return nullptr;
}

bool UUGCFunctionLibary::ProjectWorldToScreen(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative)
{
    ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
    if (LP && LP->ViewportClient)
    {
        // get the projection data
        FSceneViewProjectionData ProjectionData;
        if (LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
        {
            FMatrix const ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
            FIntRect ViewRect = ProjectionData.GetConstrainedViewRect();

            FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
            // the result of this will be x and y coords in -1..1 projection space
            const float RHW = 1.0f / Result.W;
            FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

            // Move from projection space to normalized 0..1 UI space
            const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
            const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

            FVector2D RayStartViewRectSpace(
                (NormalizedX * (float)ViewRect.Width()),
                (NormalizedY * (float)ViewRect.Height())
            );

            ScreenPosition = RayStartViewRectSpace + FVector2D(static_cast<float>(ViewRect.Min.X), static_cast<float>(ViewRect.Min.Y));

            return true;
        }
    }

    ScreenPosition = FVector2D::ZeroVector;
    return false;
}

FVector2D UUGCFunctionLibary::ScreenToWindow(const FVector2D ScreenPosition)
{
// 将屏幕坐标转换成相对View的坐标
    if (GEngine && GEngine->GameViewport)
    {
        TSharedPtr<IGameLayerManager> GameLayerManager = GEngine->GameViewport->GetGameLayerManager();
        if (GameLayerManager.IsValid())
        {
            const FGeometry& ViewportGeometry = GameLayerManager->GetViewportWidgetHostGeometry();
            FVector2D  LocalPos(ScreenPosition.X - ViewportGeometry.AbsolutePosition.X, ScreenPosition.Y - ViewportGeometry.AbsolutePosition.Y);
            return LocalPos;
        }
    }
    return ScreenPosition;
}

int UUGCFunctionLibary::GetClientViewWidth()
{
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        return ViewportSize.X;
    }
    return 0;
}

int UUGCFunctionLibary::GetClientViewHeight()
{
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        return ViewportSize.Y;
    }
    return 0;
}

bool UUGCFunctionLibary::LineBoxIntersection(const FBox& Box, const FVector& Start, const FVector& End, const FVector& Direction)
{
    return FMath::LineBoxIntersection(Box, Start, End, Direction);
}

bool UUGCFunctionLibary::LineSphereIntersection(const FVector& Start, const FVector& Dir, float Length, const FVector& Origin, float Radius)
{
    return FMath::LineSphereIntersection<double>(Start, Dir, Length, Origin, Radius);
}

bool UUGCFunctionLibary::SphereIntersection(FVector& OutHit, const FVector& Start, const FVector& Dir, const FVector& Origin, float Radius)
{
    FVector   v = Start;   // v = vBegin - m_vCenter;
    v -= Origin;

    float     b = 2.0f * Dir.Dot(v);
    float     c = v.Dot(v) - Radius * Radius;
    float  fDis = (b * b) - (4.0f * c);

    if (fDis < 0.0f)
        return false;
    fDis = FMath::Sqrt(fDis);
    float  fFar = (-b + fDis) * 0.5f;
    float  fNear = (-b - fDis) * 0.5f;
    if (fFar < 0.0f || fNear < 0.0f)
        return false;
    OutHit = Start;
    OutHit += Dir * fNear;
    return true;
}