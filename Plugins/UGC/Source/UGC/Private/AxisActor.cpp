// Copyright Epic Games, Inc. All Rights Reserved.
#include "AxisActor.h"
#include "Components/SceneComponent.h"
#include "AxisBoxComponent.h"
#include "AxisTranstromComponent.h"
#include "AxisRotationComponent.h"

AAxisActor::AAxisActor()
{
    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(SceneComponent);

    BoxComponent = CreateDefaultSubobject<UAxisBoxComponent>(TEXT("Box"));
    BoxComponent->SetupAttachment(SceneComponent);

    TransfromComponentZ = CreateDefaultSubobject<UAxisTranstromComponent>(TEXT("TransformZ"));
    TransfromComponentZ->SetupAttachment(SceneComponent);
    TransfromComponentZ->Color = FLinearColor::Blue;

    UAxisTranstromComponent *TransX = CreateDefaultSubobject<UAxisTranstromComponent>(TEXT("TransformX"));
    TransX->SetupAttachment(SceneComponent);
    TransX->Direction = FVector(1, 0, 0);
    TransfromComponentX = TransX;
    TransfromComponentX->Color = FLinearColor::Blue;

    UAxisTranstromComponent* TransY = CreateDefaultSubobject<UAxisTranstromComponent>(TEXT("TransformY"));
    TransY->SetupAttachment(SceneComponent);
    TransY->Direction = FVector(0, 1, 0);
    TransfromComponentY = TransY;
    TransfromComponentY->Color = FLinearColor::Blue;

    UAxisRotationComponent *RotationComponent = CreateDefaultSubobject<UAxisRotationComponent>(TEXT("RotaitonX"));
    RotationComponent->Normal = FVector(1, 0, 0);
    RotationComponent->Color = FLinearColor(0, 0.13286, 1, 1);
    RotationComponentX = RotationComponent;
    RotationComponentX->SetupAttachment(SceneComponent);

    RotationComponent = CreateDefaultSubobject<UAxisRotationComponent>(TEXT("RotaitonY"));
    RotationComponent->Normal = FVector(0, 1, 0);
    RotationComponent->Color = FLinearColor(0.337497, 0.594882, 1, 1);
    RotationComponentY = RotationComponent;
    RotationComponentY->SetupAttachment(SceneComponent);

    RotationComponent = CreateDefaultSubobject<UAxisRotationComponent>(TEXT("RotaitonZ"));
    RotationComponent->Normal = FVector(0, 0, 1);
    RotationComponent->Color = FLinearColor(1, 0, 0, 1);
    RotationComponentZ = RotationComponent;
    RotationComponentZ->SetupAttachment(SceneComponent);
}
