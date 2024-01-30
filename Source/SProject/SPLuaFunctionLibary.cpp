#include "SPLuaFunctionLibary.h"
#include "Engine/World.h"
#include "SceneView.h"


bool USPLuaFunctionLibary::GetRenderTextureColors(TArray<FColor>& OutColors, UTextureRenderTarget2D* SourceRenderTarget)
{
    if(SourceRenderTarget)
    {
        FRenderTarget* RenderTarget = SourceRenderTarget->GameThread_GetRenderTargetResource();
        return RenderTarget->ReadPixels(OutColors);
    }
    return false;
}

FVector2D  USPLuaFunctionLibary::GetLastMousePosition()
{
    FVector2D  MousePosition;
    if(GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetMousePosition(MousePosition);
    }
    return MousePosition;
}

void  USPLuaFunctionLibary::CheckPostion(const FVector& InPos)
{
    float X = InPos.X;
    float Y = InPos.Y;
}

void  USPLuaFunctionLibary::SetActorSelectEffect(AActor* InActor)
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

APostProcessVolume*  USPLuaFunctionLibary::CreatePostProcessActor(const FPostProcessSettings &PostProcessSettings)
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

bool USPLuaFunctionLibary::ProjectWorldToScreen(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative)
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

int USPLuaFunctionLibary::GetClientViewWidth()
{
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        return ViewportSize.X;
    }
    return 0;
}

int USPLuaFunctionLibary::GetClientViewHeight()
{
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        return ViewportSize.Y;
    }
    return 0;
}
