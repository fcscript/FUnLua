
#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/PlayerController.h"
#include "SPLuaFunctionLibary.generated.h"

UCLASS()
class  USPLuaFunctionLibary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static bool GetRenderTextureColors(TArray<FColor> &OutColors, UTextureRenderTarget2D *SourceRenderTarget);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static FVector2D  GetLastMousePosition();

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static void  CheckPostion(const FVector& InPos);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static void  SetActorSelectEffect(AActor * InActor);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static APostProcessVolume*  CreatePostProcessActor(const FPostProcessSettings &PostProcessSettings);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static bool ProjectWorldToScreen(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static int GetClientViewWidth();

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "SPLuaFunctionLibary"))
    static int GetClientViewHeight();
};
