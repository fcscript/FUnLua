#pragma once
#include "../LuaCore/LuaHeader.h"

#include "Containers/Map.h"
#include "FCTemplateType.h"
#include "FCObjectManager.h"


struct FTMapCacheBuffer
{
    uint8  TempBuffer[128];
    uint8* Buffer;
    FTMapCacheBuffer(FProperty* Prop)
    {
        Buffer = TempBuffer;

        FStructBuilder StructBuilder;
        StructBuilder.AddMember(Prop->GetSize(), Prop->GetMinAlignment());
        // allocate cache for a key-value pair with alignment
        int nMaxBufferSize = StructBuilder.GetSize();
        if (nMaxBufferSize < StructBuilder.GetAlignment())
        {
            nMaxBufferSize = StructBuilder.GetAlignment();
        }
        if (nMaxBufferSize > sizeof(TempBuffer))
        {
            Buffer = (uint8*)FMemory::Malloc(StructBuilder.GetSize(), StructBuilder.GetAlignment());
        }
    }
    ~FTMapCacheBuffer()
    {
        if (Buffer != TempBuffer)
        {
            FMemory::Free(Buffer);
        }
    }
};

struct FTMapKeyValueBuffer : public FTMapCacheBuffer
{
    FProperty* Prop;
    FCDynamicProperty* ElementProperty;
    FTMapKeyValueBuffer(FProperty* InProp) :FTMapCacheBuffer(InProp), Prop(InProp)
    {
        InProp->InitializeValue(Buffer);
        ElementProperty = GetDynamicPropertyByUEProperty(InProp);
    }
    FTMapKeyValueBuffer(FProperty* InProp, lua_State* L, int Idx) :FTMapCacheBuffer(InProp), Prop(InProp)
    {
        InProp->InitializeValue(Buffer);
        ElementProperty = GetDynamicPropertyByUEProperty(InProp);
        ElementProperty->m_ReadScriptFunc(L, Idx, ElementProperty, Buffer, nullptr, nullptr);
    }
    void ReadScriptValue(lua_State* L, int Idx)
    {
        ElementProperty->m_ReadScriptFunc(L, Idx, ElementProperty, Buffer, nullptr, nullptr);
    }
    ~FTMapKeyValueBuffer()
    {
        Prop->DestroyValue(Buffer);
    }
};