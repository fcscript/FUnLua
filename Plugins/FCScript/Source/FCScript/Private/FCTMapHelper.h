#pragma once
#include "FCBrigeHelper.h"
#include "Containers/Map.h"
#include "FCTemplateType.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"

class FCTMapHelper
{
    FScriptMap* ScriptMap;
    FMapProperty* MapProperty;
public:
    FCTMapHelper() :ScriptMap(nullptr), MapProperty(nullptr)
    {
    }
    FCTMapHelper(FScriptMap* InScriptMap, const FCDynamicProperty* InDynamicProperty)
    {
        ScriptMap = InScriptMap;
        MapProperty = (FMapProperty*)InDynamicProperty->Property;
    }
    FCTMapHelper(FCObjRef* ObjRef) :ScriptMap(nullptr), MapProperty(nullptr)
    {
        if (ObjRef && ObjRef->DynamicProperty)
        {
            if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Map)
            {
                ScriptMap = (FScriptMap*)ObjRef->GetThisAddr();
                MapProperty = (FMapProperty*)ObjRef->DynamicProperty->Property;
            }
        }
    }
public:
    bool IsValid() const
    {
        return ScriptMap != nullptr;
    }
public:
    void  Copy(const FScriptMap* OtherArray);
};