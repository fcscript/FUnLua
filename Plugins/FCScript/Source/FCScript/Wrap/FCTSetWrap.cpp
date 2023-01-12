#include "FCTSetWrap.h"
#include "Containers/Map.h"
#include "FCTemplateType.h"

#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "FTMapKeyValueBuffer.h"
#include "../LuaCore/LuaContext.h"
#include "FCTSetHelper.h"

void FCTSetWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TSet", LibOpen_wrap, 1);
    SetWrapClassName("TSet");
}

int FCTSetWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "GetNumb", GetNumb_wrap },
        { "Contains", Contains_wrap },
        { "Add", Add_wrap },
        { "Remove", Remove_wrap },
        { "Clear", Clear_wrap },
        { "ToArray", ToArray_wrap },
        { "SetArray", SetArray_wrap },
        { "Length", GetNumb_wrap },
        { "size", GetNumb_wrap },
        { "push_back", Add_wrap },
        { "GetMaxIndex", GetMaxIndex_wrap },
        { "ToNextValidIndex", ToNextValidIndex_wrap },
        { "IsValidIndex", IsValidIndex_wrap },
        { "GetAt", GetAt_wrap },
        { "RemoveAt", RemoveAt_wrap },

        { "__gc", FCExportedClass::obj_Delete },
        { "__call", obj_new },
        { "__eq", FCExportedClass::obj_equal },
        { "__pairs", obj_pairs },
        { "__len", GetNumb_wrap },
        { nullptr, nullptr }
    };
    FCExportedClass::RegisterLibClass(L, "TSet", LibFuncs);
    return 1;
}

// pair 的生命周期要短于Map对象，所以可以这里面直接保存TMap吧
struct FCTSet_Pairs : public FCTSetHelper
{
    FCObjRef* ObjRef;
    int    PairIndex; // TMap第一个有效的是0
    int    PairCount;

    FCDynamicProperty  ValueProperty;
    FCTSet_Pairs(FCObjRef* InObjRef) :ObjRef(InObjRef), PairIndex(-1), PairCount(0)
    {
    }
    void InitProperty()
    {
        Init(ObjRef);
        if (IsValid())
        {
            ValueProperty.InitProperty(SetProperty->ElementProp);
        }
    }
};

int FCTSetWrap::pair_gc(lua_State* L)
{
    FCTSet_Pairs* Pairs = (FCTSet_Pairs*)lua_touserdata(L, 1);
    if (Pairs)
    {
        Pairs->~FCTSet_Pairs(); // 只需要执行析构就行了
    }
    return 0;
}

int FCTSetWrap::obj_NextPairs(lua_State* L)
{
    FCTSet_Pairs* Pairs = (FCTSet_Pairs*)lua_touserdata(L, 1);
    if (Pairs->IsValid())
    {
        int NextIndex = Pairs->GetNextValidIndex(Pairs->PairIndex + 1);
        Pairs->PairIndex = NextIndex;
        if (Pairs->IsValidIndex(NextIndex))
        {
            Pairs->PairCount++;
            lua_pushinteger(L, NextIndex);
            const FScriptSetLayout& SetLayout = Pairs->SetProperty->SetLayout;
            uint8* Result = (uint8*)Pairs->ScriptSet->GetData(NextIndex, SetLayout);
            Pairs->ValueProperty.m_WriteScriptFunc(L, &Pairs->ValueProperty, Result, nullptr, nullptr);
            return 2;
        }
    }
    return 0;
}

int FCTSetWrap::obj_pairs(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    lua_pushcfunction(L, obj_NextPairs);
    FCTSet_Pairs* Pairs = (FCTSet_Pairs*)lua_newuserdata(L, sizeof(FCTSet_Pairs));
    Pairs = new(Pairs) FCTSet_Pairs(ObjRef);
    Pairs->InitProperty();

    lua_newtable(L);
    lua_pushcfunction(L, pair_gc);
    lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushnil(L);

    return 3;    
}

int FCTSetWrap::obj_new(lua_State* L)
{
    const char* KeyTypeName = GetPropertyType(L, 2);
    FCDynamicProperty* DynamicProperty = GetTSetDynamicProperty(KeyTypeName);
    if (DynamicProperty)
    {
        FScriptSet* ScriptMap = new FScriptSet();
        int64 ObjID = FCGetObj::GetIns()->PushTemplate(DynamicProperty, ScriptMap, EFCObjRefType::NewTSet);
        FCScript::PushBindObjRef(L, ObjID, "TSet");
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

int FCTSetWrap::GetNumb_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef *)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        int Num = ScriptMap->Num();
        lua_pushinteger(L, Num);
    }
    else
    {
        lua_pushinteger(L, 0);
    }
    return 1;
}

int FCTSetWrap::Contains_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int32 FindIndex = INDEX_NONE;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;

        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();

        FTMapKeyValueBuffer KeyBuffer(ElementProp, L, 2);

        FindIndex = ScriptMap->FindIndex(KeyBuffer.Buffer, SetProperty->SetLayout,
            [ElementProp](const void* ElementKey) { return ElementProp->GetValueTypeHash(ElementKey); },
            [ElementProp](const void* A, const void* B) { return ElementProp->Identical(A, B); }
        );
    }
    lua_pushboolean(L, FindIndex != INDEX_NONE);
    return 1;
}

int FCTSetWrap::Add_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int32 FindIndex = INDEX_NONE;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;

        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();

        FTMapKeyValueBuffer KeyValueBuffer(ElementProp, L, 2);
        void* ValueBuffer = KeyValueBuffer.Buffer;

        FindIndex = ScriptMap->FindOrAdd(ValueBuffer, SetLayout,
            [ElementProp](const void* ElementKey) { return ElementProp->GetValueTypeHash(ElementKey); },
            [ElementProp](const void* A, const void* B) { return ElementProp->Identical(A, B); },
            [ElementProp, ValueBuffer](void* NewElement)
        {
            ElementProp->InitializeValue(NewElement);
            ElementProp->CopySingleValue(NewElement, ValueBuffer);
        }
        );
    }
    return 0;
}

int FCTSetWrap::Remove_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int32 FindIndex = INDEX_NONE;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;

        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();

        FTMapKeyValueBuffer KeyBuffer(ElementProp, L, 2);

        FindIndex = ScriptMap->FindIndex(KeyBuffer.Buffer, SetProperty->SetLayout,
            [ElementProp](const void* ElementKey) { return ElementProp->GetValueTypeHash(ElementKey); },
            [ElementProp](const void* A, const void* B) { return ElementProp->Identical(A, B); }
        );

        if (FindIndex != INDEX_NONE)
        {
            uint8* Result = (uint8*)ScriptMap->GetData(FindIndex, SetLayout);
            ElementProp->DestroyValue(Result);
            ScriptMap->RemoveAt(FindIndex, SetLayout);
        }
    }

    return 0;
}

int FCTSetWrap::Clear_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int32 FindIndex = INDEX_NONE;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        TSet_Clear(ScriptMap, SetProperty);
    }
    return 0;
}

int FCTSetWrap::ToArray_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int32 FindIndex = INDEX_NONE;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;

        FCDynamicProperty  ValueProperty;
        ValueProperty.InitProperty(ElementProp);

        int Num = ScriptMap->Num();

        lua_createtable(L, 0, 0);
        int nTableIdx = lua_gettop(L);

        int ArrayIndex = 0;
        int32  MaxIndex = ScriptMap->GetMaxIndex();
        for (int32 PairIndex = 0; PairIndex < MaxIndex; ++PairIndex)
        {
            if (ScriptMap->IsValidIndex(PairIndex))
            {
                uint8* Result = (uint8*)ScriptMap->GetData(PairIndex, SetLayout);
                ValueProperty.m_WriteScriptFunc(L, &ValueProperty, Result, nullptr, nullptr);
                lua_rawseti(L, nTableIdx, ++ArrayIndex);
            }
        }
        return 1;
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

struct TSetSetArrayCallbackInfo
{
    FCObjRef* ObjRef;
    FSetProperty* SetProperty;
    FScriptSet* ScriptMap;
    FTMapKeyValueBuffer* KeyValueBuffer;
};

void  TSetSetArray_Callback(lua_State* L, int Index, void* UserData)
{
    TSetSetArrayCallbackInfo* Info = (TSetSetArrayCallbackInfo*)UserData;
    FSetProperty* SetProperty = Info->SetProperty;
    FProperty* ElementProp = SetProperty->ElementProp;
    const FScriptSetLayout& SetLayout = SetProperty->SetLayout;
    FTMapKeyValueBuffer* KeyValueBuffer = Info->KeyValueBuffer;

    KeyValueBuffer->ReadScriptValue(L, -2);
    void* ValueBuffer = KeyValueBuffer->Buffer;
    Info->ScriptMap->FindOrAdd(ValueBuffer, SetLayout,
        [ElementProp](const void* ElementKey) { return ElementProp->GetValueTypeHash(ElementKey); },
        [ElementProp](const void* A, const void* B) { return ElementProp->Identical(A, B); },
        [ElementProp, ValueBuffer](void* NewElement)
        {
            ElementProp->InitializeValue(NewElement);
            ElementProp->CopySingleValue(NewElement, ValueBuffer);
        }
    );
}

int FCTSetWrap::SetArray_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int32 FindIndex = INDEX_NONE;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        int32 Type = lua_rawget(L, 2);
        if (Type != LUA_TTABLE)
        {
            return 0;
        }
        int nTableIdx = 2;
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;

        TSet_Clear(ScriptMap, SetProperty);

        FTMapKeyValueBuffer KeyValueBuffer(ElementProp);

        TSetSetArrayCallbackInfo  Info = { ObjRef,  SetProperty , ScriptMap , &KeyValueBuffer };
        LoopTable(L, nTableIdx, TSetSetArray_Callback, &Info);
     }

    return 0;
}

int FCTSetWrap::GetMaxIndex_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int MaxIndex = 0;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        MaxIndex = ScriptMap->GetMaxIndex();
    }
    lua_pushinteger(L, MaxIndex);
    return 1;
}

int FCTSetWrap::ToNextValidIndex_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCTSetHelper Helper(ObjRef);
    int ValidIndex = 0;
    if(Helper.IsValid())
    {
        int32 PairIndex = lua_tointeger(L, 2);
        ValidIndex = Helper.GetNextValidIndex(PairIndex);
    }
    lua_pushinteger(L, ValidIndex);
    return 1;
}

int FCTSetWrap::IsValidIndex_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    bool bValid = false;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        int32 PairIndex = lua_tointeger(L, 2);
        bValid = ScriptMap->IsValidIndex(PairIndex);
    }
    lua_pushboolean(L, bValid);
    return 1;
}

int FCTSetWrap::GetAt_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;

        FCDynamicProperty  ValueProperty;
        ValueProperty.InitProperty(ElementProp);

        int32 PairIndex = lua_tointeger(L, 2);
        if (ScriptMap->IsValidIndex(PairIndex))
        {
            uint8* Result = (uint8*)ScriptMap->GetData(PairIndex, SetLayout);
            ValueProperty.m_WriteScriptFunc(L, &ValueProperty, Result, nullptr, nullptr);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

int FCTSetWrap::RemoveAt_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int ValidIndex = 0;
    if (ObjRef && ObjRef->DynamicProperty && ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
    {
        FSetProperty* SetProperty = (FSetProperty*)ObjRef->DynamicProperty->Property;
        FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
        FProperty* ElementProp = SetProperty->ElementProp;
        const FScriptSetLayout& SetLayout = SetProperty->SetLayout;
        int32 PairIndex = lua_tointeger(L, 2);
        if(ScriptMap->IsValidIndex(PairIndex))
        {
            uint8* Result = (uint8*)ScriptMap->GetData(PairIndex, SetLayout);
            ElementProp->DestroyValue(Result);
            ScriptMap->RemoveAt(PairIndex, SetLayout);
        }
    }
    return 0;
}
