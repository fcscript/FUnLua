#include "TLazyObjectPtrWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"

void TLazyObjectPtrWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TLazyObjectPtr", LibOpen_wrap, 1);
}

int TLazyObjectPtrWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Reset", Reset_wrap },
        { "ResetWeakPtr", ResetWeakPtr_wrap },
        { "IsValid", IsValid_wrap },
        { "IsPending", IsPending_wrap },
        { "Get", Get_wrap },
        { "Set", Set_wrap },

        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    FCExportedClass::RegisterLibClass(L, "TLazyObjectPtr", LibFuncs);
    return 1;
}

int TLazyObjectPtrWrap::Reset_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_LazyObjectPtr)
        {
			FLazyObjectPtr *ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();
            ScriptPtr->Reset();
        }        
    }
    return 0;
}

int TLazyObjectPtrWrap::ResetWeakPtr_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_LazyObjectPtr)
        {
            FLazyObjectPtr* ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();
            ScriptPtr->ResetWeakPtr();
        }
    }
    return 0;
}

int TLazyObjectPtrWrap::IsValid_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    bool bValidPtr = false;
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_LazyObjectPtr)
        {
			FLazyObjectPtr *ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();
            bValidPtr = ScriptPtr->IsValid();
        }        
    }
    lua_pushboolean(L, bValidPtr);
    return 1;
}

int TLazyObjectPtrWrap::IsPending_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    bool bValidPtr = false;
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_LazyObjectPtr)
        {
            FLazyObjectPtr* ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();
            bValidPtr = ScriptPtr->IsPending();
        }
    }
    lua_pushboolean(L, bValidPtr);
    return 1;
}

int TLazyObjectPtrWrap::Get_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_LazyObjectPtr)
        {
            FLazyObjectPtr* ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();
            UObject* Obj = ScriptPtr->Get();
            FCScript::PushUObject(L, Obj);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

int TLazyObjectPtrWrap::Set_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_LazyObjectPtr)
        {
            FLazyObjectPtr* ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();

            int64 Arg1 = (int64)lua_touserdata(L, 2);
            UObject* Obj = FCGetObj::GetIns()->GetUObject(Arg1);
            *ScriptPtr = Obj;
        }
    }
    return 0;
}

