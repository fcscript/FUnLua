#pragma once
#include "FCBrigeHelper.h"
#include "Containers/ScriptArray.h"
#include "FCTemplateType.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"


struct TArrayCacheValue
{
    uint8 Buffer[64];
    uint8* ElementCache;
    FScriptArray* ScriptArray;
    FCDynamicProperty* ElementProperty;
    FProperty* Inner;
    bool   bValid;
    TArrayCacheValue()
    {
        ElementCache = ElementCache;
        ScriptArray = nullptr;
        ElementProperty = nullptr;
        Inner = nullptr;
        bValid = false;
    }
    bool  ReadValue(lua_State* L, FCObjRef* ObjRef, int Idx)
    {
        bValid = false;
        if (Inner)
        {
            Inner->DestroyValue(ElementCache);
        }
        if (ObjRef && ObjRef->DynamicProperty)
        {
            if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
            {
                ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
                FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
                Inner = ArrayProperty->Inner;

                int ElementSize = Inner->GetSize();
                int Alignment = Inner->GetMinAlignment();
                if (Alignment < sizeof(void*))
                    Alignment = sizeof(void*);
                if (ElementSize > 64)
                {
                    ElementCache = (uint8*)FMemory::Malloc(ElementSize, Alignment);
                }
                ArrayProperty->InitializeValue(ElementCache);

                ElementProperty = GetDynamicPropertyByUEProperty(Inner);
                ElementProperty->m_ReadScriptFunc(L, Idx, ElementProperty, ElementCache, NULL, ObjRef);
                bValid = true;
            }
        }
        return bValid;
    }
    ~TArrayCacheValue()
    {
        if (Inner)
        {
            Inner->DestroyValue(ElementCache);
        }
        if (ElementCache != Buffer)
        {
            FMemory::Free(ElementCache);
        }
    }
};

class FCTArrayHelper
{
    FScriptArray  * ScriptArray;
    FArrayProperty* ArrayProperty;
    int ElementSize;
public:
    FCTArrayHelper():ScriptArray(nullptr), ArrayProperty(nullptr), ElementSize(0)
    {

    }
    FCTArrayHelper(FScriptArray *InScriptArray, const FCDynamicProperty *InDynamicProperty)
    {
        ScriptArray = InScriptArray;
        ArrayProperty = (FArrayProperty*)InDynamicProperty->Property;
        ElementSize = ArrayProperty->Inner->GetSize();
    }
    FCTArrayHelper(FCObjRef* ObjRef) :ScriptArray(nullptr), ArrayProperty(nullptr), ElementSize(0)
    {
        if (ObjRef && ObjRef->DynamicProperty)
        {
            if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
            {
                ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
                ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
                ElementSize = ArrayProperty->Inner->GetSize();
            }
        }
    }
public:
    bool IsValid() const
    {
        return ScriptArray != nullptr;
    }
public:
    void  Resize(int NewNum);
    void  Copy(const FScriptArray *OtherArray);
};