#pragma once
#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject.h"
#include "FCStringCore.h"

struct FCSafeProperty
{
    const FProperty* Property;
    TWeakFieldPtr<FProperty> PropertyPtr;
    int ArrayDim;

    FCSafeProperty();
    virtual ~FCSafeProperty(){}
    virtual void InitProperty(const FProperty* InProperty);

    UStruct *GetOwnerStruct();
};

FCSafeProperty  *GetSafeProperty(const FProperty* InProperty);
void ClearAllSafeProperty();