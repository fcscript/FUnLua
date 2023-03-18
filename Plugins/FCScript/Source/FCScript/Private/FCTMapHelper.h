#pragma once
#include "FCBrigeHelper.h"
#include "Containers/Map.h"
#include "FCTemplateType.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"

class FCTMapHelper
{
public:
    FScriptMap* ScriptMap;
    FMapProperty* MapProperty;
public:
    FCTMapHelper() :ScriptMap(nullptr), MapProperty(nullptr)
    {
    }
    FCTMapHelper(FScriptMap* InScriptMap, const FCDynamicProperty* InDynamicProperty)
    {
        ScriptMap = InScriptMap;
        MapProperty = InDynamicProperty->SafePropertyPtr->CastMapProperty();
    }
    FCTMapHelper(FCObjRef* ObjRef) :ScriptMap(nullptr), MapProperty(nullptr)
    {
        if (ObjRef && ObjRef->DynamicProperty)
        {
            if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Map)
            {
                ScriptMap = (FScriptMap*)ObjRef->GetThisAddr();
                MapProperty = ObjRef->DynamicProperty->SafePropertyPtr->CastMapProperty();
            }
        }
    }
public:
    bool IsValid() const
    {
        return ScriptMap != nullptr;
    }
    int GetMaxIndex() const
    {
        return ScriptMap ? ScriptMap->GetMaxIndex() : 0;
    }
    bool IsValidIndex(int32 Index) const
    {
        if(ScriptMap)
            return ScriptMap->IsValidIndex(Index);
        else
            return false;
    }
public:
    void  Copy(const FScriptMap* OtherArray);
};