#include "FCSafeProperty.h"

FCSafeProperty::FCSafeProperty():Property(nullptr)
    , PropertyPtr(nullptr)
    , ArrayDim(1)
{
    
}

void FCSafeProperty::InitProperty(const FProperty* InProperty)
{
    Property = InProperty;
    PropertyPtr = InProperty;
    ArrayDim = InProperty->ArrayDim;
}

UStruct* FCSafeProperty::GetOwnerStruct()
{
    if(PropertyPtr.IsValid())
    {
        return Property->GetOwnerStruct();
    }
    return nullptr;
}


typedef  std::unordered_map<const FProperty*, FCSafeProperty*>   CSafeProperyMap; // FProperty* ==> FCSafeProperty

CSafeProperyMap  GSafePropertyMap;

FCSafeProperty* GetSafeProperty(const FProperty* InProperty)
{
    CSafeProperyMap::iterator itProperty = GSafePropertyMap.find(InProperty);
    if(itProperty != GSafePropertyMap.end())
    {
        return itProperty->second;
    }

    FCSafeProperty  *SafeProperty = new FCSafeProperty();
    SafeProperty->InitProperty(InProperty);
    return SafeProperty;
}

void ClearAllSafeProperty()
{
    ReleasePtrMap(GSafePropertyMap);
}