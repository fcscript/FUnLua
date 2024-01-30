
#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/PlayerController.h"
#include "UGCFunctionLibary.generated.h"

UCLASS()
class  UUGCFunctionLibary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static bool GetRenderTextureColors(TArray<FColor> &OutColors, UTextureRenderTarget2D *SourceRenderTarget);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static FVector2D  GetLastMousePosition();

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static void  SetActorSelectEffect(AActor * InActor);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static APostProcessVolume*  CreatePostProcessActor(const FPostProcessSettings &PostProcessSettings);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static bool ProjectWorldToScreen(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static FVector2D ScreenToWindow(const FVector2D ScreenPosition);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static int GetClientViewWidth();

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static int GetClientViewHeight();

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static bool LineBoxIntersection(const FBox& Box, const FVector& Start, const FVector& End, const FVector& Direction);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static bool LineSphereIntersection(const FVector& Start, const FVector& Dir, float Length, const FVector& Origin, float Radius);

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "UGCFunctionLibary"))
    static bool SphereIntersection(FVector &OutHit, const FVector& Start, const FVector& Dir, const FVector& Origin, float Radius);
};
