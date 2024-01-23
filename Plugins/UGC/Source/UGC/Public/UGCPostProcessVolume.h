// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/PostProcessVolume.h"
#include "UGCPostProcessVolume.generated.h"

UCLASS(Blueprintable, ClassGroup = Utility, HideCategories = (Physics, Collision, Mobile))
class UGC_API AUGCPostProcessVolume : public APostProcessVolume
{
    GENERATED_BODY()
public:
    AUGCPostProcessVolume();
};
