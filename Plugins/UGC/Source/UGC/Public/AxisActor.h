// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/PrimitiveComponent.h"
#include "AxisBaseComponent.h"
#include "GameFramework/Actor.h"
#include "AxisActor.generated.h"


UCLASS(ClassGroup = Utility, HideCategories = (Physics, Collision, Mobile))
class UGC_API AAxisActor : public AActor
{
    GENERATED_BODY()
public:
    AAxisActor();
public:
    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* BoxComponent;

    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* TransfromComponentX;
    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* TransfromComponentY;
    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* TransfromComponentZ;
    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* RotationComponentX;
    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* RotationComponentY;
    UPROPERTY(EditAnywhere, Category = Options)
    UAxisBaseComponent* RotationComponentZ;

    UPROPERTY(EditAnywhere, Category = Options)
    UActorComponent* DebugLineComponent;
};
