#include "TSoftObjectPtrWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"


void TSoftObjectPtrWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TSoftObjectPtr", LibOpen_wrap, 1);
}

int TSoftObjectPtrWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "LoadSynchronous", LoadSynchronous_wrap },
        { "GetAssetName", GetAssetName_wrap },

        { "__gc", FCExportedClass::obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    FCExportedClass::RegisterLibClass(L, "TSoftObjectPtr", LibFuncs);
    return 1;
}

int TSoftObjectPtrWrap::LoadSynchronous_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_WeakObjectPtr)
        {
			FSoftObjectPtr *ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
			FSoftObjectProperty  *Property = (FSoftObjectProperty *)ObjRef->DynamicProperty->Property;
            UClass *ObjClass = Property->PropertyClass;
            UObject *Obj = ScriptPtr->LoadSynchronous();
            FCScript::PushUObject(L, Obj);
            return 1;
        }        
    }
    lua_pushnil(L);
    return 0;
}

int TSoftObjectPtrWrap::GetAssetName_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_WeakObjectPtr)
        {
            FSoftObjectPtr* ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
            FString  AssetName = ScriptPtr->GetAssetName();
            lua_pushstring(L, TCHAR_TO_UTF8(*AssetName));
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}
