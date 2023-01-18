#include "TWeakObjectPtrWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCTemplateType.h"


void TWeakObjectPtrWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TWeakObjectPtr", LibOpen_wrap, 1);
    luaL_requiref(L, "WeakPtr", LibOpen_wrap, 1);
}

int TWeakObjectPtrWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Reset", Reset_wrap },
        { "IsValid", IsValid_wrap },
        { "Get", Get_wrap },
        { "Set", Set_wrap },

        { "__call", obj_New },
        { "__gc", FCExportedClass::obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    const char* ClassName = lua_tostring(L, 1);
    return FCExportedClass::RegisterLibClass(L, ClassName, LibFuncs);
}

int TWeakObjectPtrWrap::obj_New(lua_State* L)
{
    // TLazyObjectPtr(UObject)
    UObject* Object = FCScript::GetUObject(L, 2);
    FWeakObjectPtr* ScriptPt = new FWeakObjectPtr(Object);
    FCDynamicProperty* DynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_WeakObjectPtr, "TWeakObjectPtr", sizeof(FWeakObjectPtr));
    int64 ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)DynamicProperty, ScriptPt, EFCObjRefType::NewTWeakPtr);
    FCScript::PushBindObjRef(L, ObjID, "TWeakObjectPtr");
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

