#include "SPLuaFunctionLibary.h"


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
