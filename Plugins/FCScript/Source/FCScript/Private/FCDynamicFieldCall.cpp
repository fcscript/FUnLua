#include "FCDynamicClassDesc.h"
#include "UObject/Class.h"
#include "FCCallScriptFunc.h"
#include "FCDynamicOverrideFunc.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "../LuaCore/LuaContext.h"
#include "../LuaCore/CallLua.h"

int FCDynamicProperty::DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    uint8* ObjAddr = (uint8*)(ObjRef->GetPropertyAddr());
    uint8* ValueAddr = ObjAddr + this->Offset_Internal;
    UObject* ThisObj = ObjRef->GetUObject();
    this->m_WriteScriptFunc(L, this, ValueAddr, ThisObj, ObjRef);
    return 1;
}

int FCDynamicProperty::DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    uint8* ObjAddr = (uint8*)(ObjRef->GetPropertyAddr());
    uint8* ValueAddr = ObjAddr + this->Offset_Internal;
    UObject* ThisObj = ObjRef->GetUObject();
    this->m_ReadScriptFunc(L, 3, this, ValueAddr, ThisObj, ObjRef);
    return 0;
}

int FCDynamicFunction::DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc *)ClassDescPtr;
    lua_pushlightuserdata(L, (void*)ClassDesc);
    lua_pushlightuserdata(L, (void*)(ObjRef ? ObjRef->PtrIndex : 0));
    lua_pushlightuserdata(L, (void*)this);
    if (IsLatentFunction())
    {
        lua_pushcclosure(L, Class_CallLatentFunction, 3);   // closure
    }
    else
    {
        lua_pushcclosure(L, Class_CallFunction, 3);        // closure
    }
    return 1;
}

int FCDynamicFunction::DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    return 0;
}

int FCDynamicWrapLibFunction::DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)ClassDescPtr;
    if (m_GetFunction)
    {
        lua_pushlightuserdata(L, (void*)this);
        lua_pushlightuserdata(L, (void*)(ObjRef ? ObjRef->PtrIndex : 0));
        lua_pushlightuserdata(L, (void*)ClassDesc);
        lua_pushcclosure(L, Class_CallGetLibFunction, 3);        // closure
        return 1;
    }
    return 0;
}

int FCDynamicWrapLibFunction::DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)ClassDescPtr;
    if (m_SetFunction)
    {
        lua_pushlightuserdata(L, (void*)this);
        lua_pushlightuserdata(L, (void*)(ObjRef ? ObjRef->PtrIndex : 0));
        lua_pushlightuserdata(L, (void*)ClassDesc);
        lua_pushcclosure(L, Class_CallSetLibFunction, 3);        // closure
        return 1;
    }
    return 0;
}

int FCDynamicWrapLibAttrib::DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)ClassDescPtr;
    if (m_GetFunction)
    {
        UObject* ThisObj = nullptr;
        if (ObjRef)
        {
            ThisObj = ObjRef->GetUObject();
            if (!ThisObj && ObjRef->Parent)
                ThisObj = ObjRef->Parent->GetUObject();
        }
        return m_GetFunction(L, ObjRef, ThisObj);
    }
    return 0;
}

int FCDynamicWrapLibAttrib::DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)ClassDescPtr;
    if (m_GetFunction)
    {
        UObject* ThisObj = nullptr;
        if (ObjRef)
        {
            ThisObj = ObjRef->GetUObject();
            if (!ThisObj && ObjRef->Parent)
                ThisObj = ObjRef->Parent->GetUObject();
        }
        return m_GetFunction(L, ObjRef, ThisObj);
    }
    return 0;
}
