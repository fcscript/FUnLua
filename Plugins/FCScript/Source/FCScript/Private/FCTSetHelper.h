#pragma once
#include "FCBrigeHelper.h"
#include "Containers/Map.h"
#include "FCTemplateType.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"

class FCTSetHelper
{
public:
    FScriptSet* ScriptSet;
    FSetProperty* SetProperty;
public:
    FCTSetHelper() :ScriptSet(nullptr), SetProperty(nullptr)
    {
    }
    FCTSetHelper(FScriptSet* InScriptSet, const FCDynamicProperty* InDynamicProperty)
    {
        ScriptSet = InScriptSet;
        SetProperty = (FSetProperty*)InDynamicProperty->Property;
    }
    FCTSetHelper(FCObjRef* ObjRef) :ScriptSet(nullptr), SetProperty(nullptr)
    {
        Init(ObjRef);
    }
    void Init(FCObjRef* ObjRef)
    {
        ScriptSet = nullptr;
        SetProperty = nullptr;
        if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
        {
            ScriptSet = (FScriptSet*)ObjRef->GetThisAddr();
            SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        }
    }
public:
    bool IsValid() const
    {
        return ScriptSet != nullptr;
    }
    bool IsValidIndex(int Index) const
    {
        return ScriptSet ? ScriptSet->IsValidIndex(Index) : false;
    }
    int GetNextValidIndex(int Index)
    {
        if(!IsValid())
        {
            return Index;
        }
        int32 MaxIndex = ScriptSet->GetMaxIndex();
        int ValidIndex = MaxIndex;
        for (; Index <= MaxIndex; ++Index)
        {
            if (ScriptSet->IsValidIndex(Index))
            {
                ValidIndex = Index;
                break;
            }
        }
        return ValidIndex;
    }
public:
    void  Copy(const FScriptSet* Other);
};