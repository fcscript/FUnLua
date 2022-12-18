#pragma once
#include "FCBrigeHelper.h"
#include "Containers/Map.h"
#include "FCTemplateType.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"

class FCTSetHelper
{
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
        if (ObjRef && ObjRef->DynamicProperty)
        {
            if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
            {
                ScriptSet = (FScriptSet*)ObjRef->GetThisAddr();
                SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
            }
        }
    }
public:
    bool IsValid() const
    {
        return ScriptSet != nullptr;
    }
public:
    void  Copy(const FScriptSet* Other);
};