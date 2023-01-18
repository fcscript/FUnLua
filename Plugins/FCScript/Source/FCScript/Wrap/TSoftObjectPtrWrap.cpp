#include "TSoftObjectPtrWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCTemplateType.h"


void TSoftObjectPtrWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TSoftObjectPtr", LibOpen_wrap, 1);
    luaL_requiref(L, "SoftPtr", LibOpen_wrap, 1);
}

int TSoftObjectPtrWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "LoadSynchronous", LoadSynchronous_wrap },
        { "GetAssetName", GetAssetName_wrap },

        { "__call", obj_New },
        { "__gc", FCExportedClass::obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    const char* ClassName = lua_tostring(L, 1);
    return FCExportedClass::RegisterLibClass(L, ClassName, LibFuncs);
}

int TSoftObjectPtrWrap::obj_New(lua_State* L)
{
    // TSoftObjectPtr(AssetPath or UObject)
    FSoftObjectPtr  *SoftObjectPtr = nullptr;
    int Type = lua_type(L, 2);
    if (LUA_TSTRING == Type)
    {
        const char* ArgPath = lua_tostring(L, 2);
        FString  AssetPath(UTF8_TO_TCHAR(ArgPath));
        FSoftObjectPath  ObjPath(AssetPath);
        SoftObjectPtr = new FSoftObjectPtr(ObjPath);
    }
    else
    {
        FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
        if(ObjRef && ObjRef->DynamicProperty)
        {
            if (FCPropertyType::FCPROPERTY_SoftObjectReference == ObjRef->DynamicProperty->Type)
            {
                FSoftObjectPtr* ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
                SoftObjectPtr = new FSoftObjectPtr(*ScriptPtr);
            }
            else if (FCPROPERTY_ObjectProperty == ObjRef->DynamicProperty->Type)
            {
                SoftObjectPtr = new FSoftObjectPtr(ObjRef->GetUObject());
            }
        }
    }
    if(!SoftObjectPtr)
    {
        SoftObjectPtr = new FSoftObjectPtr();
    }
    FCDynamicProperty *DynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_SoftObjectReference, "TSoftObjectPtr", sizeof(FSoftObjectPtr));
    int64 ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)DynamicProperty, SoftObjectPtr, EFCObjRefType::NewTSoftObjectPtr);
    FCScript::PushBindObjRef(L, ObjID, "TSoftObjectPtr");
    return 1;
}

int TSoftObjectPtrWrap::LoadSynchronous_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_SoftObjectReference
            || ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_SoftClassReference)
        {
			FSoftObjectPtr *ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
            UObject *Obj = ScriptPtr->LoadSynchronous();
            FCScript::PushUObject(L, Obj);
            return 1;
        }        
    }
    lua_pushnil(L);
    return 1;
}

int TSoftObjectPtrWrap::GetAssetName_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_SoftObjectReference
            || ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_SoftClassReference)
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

//------------------------------------------

void TSoftClassPtrWrap::Register(lua_State* L)
{
    luaL_requiref(L, "TSoftClassPtr", LibOpen_wrap, 1);
}

int TSoftClassPtrWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "LoadSynchronous", TSoftObjectPtrWrap::LoadSynchronous_wrap },
        { "GetAssetName", TSoftObjectPtrWrap::GetAssetName_wrap },

        { "__call", obj_New },
        { "__gc", FCExportedClass::obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    FCExportedClass::RegisterLibClass(L, "TSoftClassPtr", LibFuncs);
    return 1;
}

int TSoftClassPtrWrap::obj_New(lua_State* L)
{
    // TSoftClassPtr(Path or UClass)
    FSoftObjectPtr* SoftObjectPtr = nullptr;
    int Type = lua_type(L, 2);
    if (LUA_TSTRING == Type)
    {
        const char* ArgPath = lua_tostring(L, 2);
        FString  AssetPath(UTF8_TO_TCHAR(ArgPath));
        FSoftObjectPath  ObjPath(AssetPath);
        SoftObjectPtr = new FSoftObjectPtr(ObjPath);
    }
    else
    {
        if (LUA_TTABLE == Type)
        {
            UStruct* Struct = FCScript::GetUStruct(L, 2);
            UClass* InClass = Cast<UClass>(Struct);
            if (InClass)
            {
                SoftObjectPtr = new FSoftObjectPtr(InClass);
            }
        }
        if(!SoftObjectPtr)
        {
            FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
            if (ObjRef && ObjRef->DynamicProperty)
            {
                if (FCPropertyType::FCPROPERTY_SoftClassReference == ObjRef->DynamicProperty->Type)
                {
                    FSoftObjectPtr* ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
                    SoftObjectPtr = new FSoftObjectPtr(*ScriptPtr);
                }
                else if (FCPROPERTY_ObjectProperty == ObjRef->DynamicProperty->Type)
                {
                    UClass* InClass = Cast<UClass>(ObjRef->GetUObject());
                    SoftObjectPtr = new FSoftObjectPtr(InClass);
                }
                else if (FCPROPERTY_ClassProperty == ObjRef->DynamicProperty->Type)
                {
                    UClass* InClass = *((UClass**)ObjRef->GetPropertyAddr());
                    SoftObjectPtr = new FSoftObjectPtr(InClass);
                }
            }
        }
    }
    if (!SoftObjectPtr)
    {
        SoftObjectPtr = new FSoftObjectPtr();
    }
    FCDynamicProperty* DynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_SoftClassReference, "TSoftClassPtr", sizeof(FSoftObjectPtr));
    int64 ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)DynamicProperty, SoftObjectPtr, EFCObjRefType::NewTSoftClassPtr);
    FCScript::PushBindObjRef(L, ObjID, "TSoftClassPtr");
    return 1;
}
