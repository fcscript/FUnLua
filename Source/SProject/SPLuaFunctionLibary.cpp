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
