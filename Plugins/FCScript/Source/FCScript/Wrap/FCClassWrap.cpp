#include "FCClassWrap.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "FCCore.h"
#include "../LuaCore/LuaContext.h"


void FCClassWrap::Register(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("UClass");
    if (ClassDesc)
    {
        ClassDesc->RegisterWrapLibFunction("Load", DoLoad_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("New", New_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("IsChildOf", DoIsChildOf_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetDefaultObject", DoGetDefaultObject_wrap, nullptr);
    }
}

const char *GetMetaClassName(lua_State* L, int Idx)
{
    int Type = lua_type(L, Idx);
    if (LUA_TSTRING == Type)
    {
        return lua_tostring(L, Idx);
    }
    else if (LUA_TTABLE == Type)
    {
        lua_pushstring(L, "__name");        // 3
        int ClassType = lua_rawget(L, -2);                 // 3
        const char* UETypeName = lua_tostring(L, -1);
        return UETypeName;
    }
    return nullptr;
}

FCDynamicClassDesc* UEUtil_FindClassDesc(const char* ClassName);
int FCClassWrap::DoLoad_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    const char* ClassName = GetMetaClassName(L, 1);
    if (!ClassName)
    {
        return 0;
    }
    FCDynamicClassDesc* ClassDesc = UEUtil_FindClassDesc(ClassName);
    FCScript::PushUObject(L, ClassDesc ? ClassDesc->m_Struct : nullptr);
    return 1;
}

int FCClassWrap::New_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
// UClass.New("Class_name")
    const char* ClassName = GetMetaClassName(L, 1);
    FCDynamicClassDesc* ClassDesc = UEUtil_FindClassDesc(ClassName);
    if(ClassDesc)
    {
        if(ClassDesc->m_Class)
        {
            EClassFlags ClassFlags = ClassDesc->m_Class->GetClassFlags();
            if(ClassDesc->m_Class->HasAnyCastFlag(CASTCLASS_AActor))
            {
                UObject* Arg2 = FCScript::GetUObject(L, 2);
                UObject* Outer = Arg2 ? Arg2 : GetTransientPackage();
                const char* ObjectName = lua_tostring(L, 3);
                FName  Name(NAME_None);
                if (ObjectName && ObjectName[0] != 0)
                {
                    Name = FName((const TCHAR*)ObjectName);
                }
                int64  ObjID = FCGetObj::GetIns()->PushNewObject(ClassDesc, Name, Outer);
                FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
                return 1;
            }
            if (ClassDesc->m_Class->HasAnyCastFlag(CASTCLASS_UClass))
            {
                int64  ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
                FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
                return 1;
            }
        }
        else if(ClassDesc->m_Struct)
        {
            int64  ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
            FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
            return 1;
        }
    }
    
    lua_pushnil(L);
    return 1;
}

int FCClassWrap::DoIsChildOf_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    UObject *Arg1 = FCScript::GetUObject(L, 1);
    UObject* Arg2 = FCScript::GetUObject(L, 2);
    UClass* A = Cast<UClass>(Arg1);
    UClass* B = Cast<UClass>(Arg2);

    lua_pushboolean(L, A && B ? A->IsChildOf(B) : false);
    return 1;
}

int FCClassWrap::DoGetDefaultObject_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    UObject* Arg1 = FCScript::GetUObject(L, 1);
    UClass* A = Cast<UClass>(Arg1);

    FCScript::PushUObject(L, A ? A->GetDefaultObject(true) : nullptr);
    return 1;
}