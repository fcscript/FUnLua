#include "TWeakObjectPtrWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"


void TWeakObjectPtrWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TWeakObjectPtr", LibOpen_wrap, 1);
}

int TWeakObjectPtrWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Reset", Reset_wrap },
        { "IsValid", IsValid_wrap },
        { "Get", Get_wrap },
        { "Set", Set_wrap },

        { "__gc", FCExportedClass::obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    FCExportedClass::RegisterLibClass(L, "TWeakObjectPtr", LibFuncs);
    return 1;
}

int TWeakObjectPtrWrap::Reset_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_WeakObjectPtr)
        {
			FWeakObjectPtr *ScriptPtr = (FWeakObjectPtr*)ObjRef->GetPropertyAddr();
            ScriptPtr->Reset();
        }
    }
    return 0;
}

int TWeakObjectPtrWrap::IsValid_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    bool bValidPtr = false;
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_WeakObjectPtr)
        {
			FWeakObjectPtr *ScriptPtr = (FWeakObjectPtr*)ObjRef->GetPropertyAddr();
            bValidPtr = ScriptPtr->IsValid();
        }
    }
    lua_pushboolean(L, bValidPtr);
    return 1;
}

int TWeakObjectPtrWrap::Get_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_WeakObjectPtr)
        {
            FWeakObjectPtr* ScriptPtr = (FWeakObjectPtr*)ObjRef->GetPropertyAddr();
            UObject *Obj = ScriptPtr->Get();
            FCScript::PushUObject(L, Obj);
            return 1;
        }
    }
    lua_pushnil(L);
    return 0;
}

int TWeakObjectPtrWrap::Set_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_WeakObjectPtr)
        {
            FWeakObjectPtr* ScriptPtr = (FWeakObjectPtr*)ObjRef->GetPropertyAddr();

            int64 Arg2 = (int64)lua_touserdata(L, 2);
            UObject *Obj = FCGetObj::GetIns()->GetUObject(Arg2);
            *ScriptPtr = Obj;
        }
    }
    return 0;
}

