#include "SPLuaFunctionLibary.h"
#include "Engine/World.h"


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
    if(GEngine && !GEngine->GameViewport.IsNull())
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
