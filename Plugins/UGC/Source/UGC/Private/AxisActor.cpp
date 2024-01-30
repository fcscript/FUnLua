// Copyright Epic Games, Inc. All Rights Reserved.
#include "AxisActor.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "AxisBoxComponent.h"
#include "AxisTranstromComponent.h"
#include "AxisRotationComponent.h"
#include "AxisDebugLineComponent.h"

AAxisActor::AAxisActor()
{
    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(SceneComponent);

    BoxComponent = CreateDefaultSubobject<UAxisBoxComponent>(TEXT("Box"));
    BoxComponent->SetupAttachment(SceneComponent);
    BoxComponent->SceneDepthGroup = SDPG_World;

    TransfromComponentZ = CreateDefaultSubobject<UAxisTranstromComponent>(TEXT("TransformZ"));
    TransfromComponentZ->SetupAttachment(SceneComponent);
    TransfromComponentZ->Color = FLinearColor::Blue;
    TransfromComponentZ->SceneDepthGroup = SDPG_World;

    UAxisTranstromComponent *TransX = CreateDefaultSubobject<UAxisTranstromComponent>(TEXT("TransformX"));
    TransX->SetupAttachment(SceneComponent);
    TransX->Direction = FVector(1, 0, 0);
    TransfromComponentX = TransX;
    TransfromComponentX->Color = FLinearColor::Blue;
    TransfromComponentX->SceneDepthGroup = SDPG_World;

    UAxisTranstromComponent* TransY = CreateDefaultSubobject<UAxisTranstromComponent>(TEXT("TransformY"));
    TransY->SetupAttachment(SceneComponent);
    TransY->Direction = FVector(0, 1, 0);
    TransfromComponentY = TransY;
    TransfromComponentY->Color = FLinearColor::Blue;
    TransfromComponentY->SceneDepthGroup = SDPG_World;

    UAxisRotationComponent *RotationComponent = CreateDefaultSubobject<UAxisRotationComponent>(TEXT("RotaitonX"));
    RotationComponent->Normal = FVector(1, 0, 0);
    RotationComponent->Color = FLinearColor(0, 0.13286, 1, 1);
    RotationComponentX = RotationComponent;
    RotationComponentX->SetupAttachment(SceneComponent);
    RotationComponentX->SceneDepthGroup = SDPG_Foreground;

    RotationComponent = CreateDefaultSubobject<UAxisRotationComponent>(TEXT("RotaitonY"));
    RotationComponent->Normal = FVector(0, 1, 0);
    RotationComponent->Color = FLinearColor(0.337497, 0.594882, 1, 1);
    RotationComponentY = RotationComponent;
    RotationComponentY->SetupAttachment(SceneComponent);
    RotationComponentY->SceneDepthGroup = SDPG_Foreground;

    RotationComponent = CreateDefaultSubobject<UAxisRotationComponent>(TEXT("RotaitonZ"));
    RotationComponent->Normal = FVector(0, 0, 1);
    RotationComponent->Color = FLinearColor(1, 0, 0, 1);
    RotationComponentZ = RotationComponent;
    RotationComponentZ->SetupAttachment(SceneComponent);
    RotationComponentZ->SceneDepthGroup = SDPG_Foreground;

    UAxisDebugLineComponent*LineComponent = CreateDefaultSubobject<UAxisDebugLineComponent>(TEXT("DebugLine"));
    LineComponent->SetupAttachment(SceneComponent);
    DebugLineComponent = LineComponent;

    LineComponent->Color = FLinearColor(0, 1, 0, 1);
    LineComponent->bRenderVisibility = false;
    LineComponent->SceneDepthGroup = SDPG_World;
}
