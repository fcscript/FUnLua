// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/PrimitiveComponent.h"
#include "AxisBaseComponent.generated.h"


UCLASS(ClassGroup = Utility, HideCategories = (Physics, Collision, Mobile))
class UGC_API UAxisBaseComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UAxisBaseComponent()
	{
		bUseEditorCompositing = false;
	}

	/**
	 * Currently this must be called if you change UProps on Base or subclass,
	 * to recreate render proxy which has a local copy of those settings
	 */
	void NotifyExternalPropertyUpdates()
	{
		MarkRenderStateDirty();
		UpdateBounds();
	}

public:
	UPROPERTY(EditAnywhere, Category = Options)
	FLinearColor Color = FLinearColor::Red;


	UPROPERTY(EditAnywhere, Category = Options)
	float HoverSizeMultiplier = 2.0f;


	UPROPERTY(EditAnywhere, Category = Options)
	float PixelHitDistanceThreshold = 7.0f;

    // gizmo visibility
    UPROPERTY(EditAnywhere, Category = Options)
    bool bRenderVisibility = true;

    UPROPERTY(EditAnywhere, Category = Options)
    bool bExternalRenderVisibility = true;

    // scale factor between pixel distances and world distances at Gizmo origin
    UPROPERTY(EditAnywhere, Category = Options)
    float DynamicPixelToWorldScale = 1.0f;

    // hover state
    UPROPERTY(EditAnywhere, Category = Options)
    bool bHovering = false;

    // world/local coordinates state
    UPROPERTY(EditAnywhere, Category = Options)
    bool bWorld = false;
public:
	UFUNCTION()
	void UpdateHoverState(bool bHoveringIn)
	{
		if (bHoveringIn != bHovering)
		{
			bHovering = bHoveringIn;
		}
	}

	UFUNCTION()
	void UpdateWorldLocalState(bool bWorldIn)
	{
		if (bWorldIn != bWorld)
		{
			bWorld = bWorldIn;
		}
	}

    UFUNCTION()
    bool IsPick(APlayerController const* Player, const FVector2D& ScreenPos, float fDist);
};
