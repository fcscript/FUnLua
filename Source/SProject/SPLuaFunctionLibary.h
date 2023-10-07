
#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SPLuaFunctionLibary.generated.h"

UCLASS()
class  USPLuaFunctionLibary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static bool GetRenderTextureColors(TArray<FColor> &OutColors, UTextureRenderTarget2D *SourceRenderTarget);
};