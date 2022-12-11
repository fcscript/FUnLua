
#include "FCSetArg.h"
#include "FCDynamicClassDesc.h"
#include "FCTemplateType.h"
#include "FCGetObj.h"
#include "FCRefObjCache.h"

void FC_SetArgValue_CppPtr(lua_State* L, const void* CppPtr)
{
    int64 PtrID = FCGetObj::GetIns()->PushCppPtr((void*)CppPtr);
    lua_pushlightuserdata(L, (void*)PtrID);
}

void FC_SetArgValue_ByName(lua_State* L, const void *ValueAddr, const char *ClassName)
{
    FCDynamicProperty *DynamicProperty = GetCppDynamicProperty(ClassName);
    if(DynamicProperty)
    {
        DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, (uint8*)ValueAddr, nullptr, nullptr);
    }
}

void* FC_GetArgValue_CppPtr(lua_State* L, int Index)
{
    int64 PtrID = (int64)lua_touserdata(L, Index);
    FCObjRef *ObjRef = FCGetObj::GetIns()->FindValue(PtrID);
    if(ObjRef && ObjRef->RefType == EFCObjRefType::CppPtr)
    {
        return ObjRef->GetThisAddr();
    }
    return nullptr;
}

void* FC_GetCppObjectPtr(lua_State* L, int Index)
{
    return FC_GetArgValue_CppPtr(L, Index);
}

void  FC_GetArgValue_ByName(lua_State* L, int Index, void* ValueAddr, const char* ClassName)
{
    FCDynamicProperty* DynamicProperty = GetCppDynamicProperty(ClassName);
    if(DynamicProperty)
    {
        DynamicProperty->m_ReadScriptFunc(L, Index, DynamicProperty, (uint8*)ValueAddr, nullptr, nullptr);
    }
}

void* FC_GetArgRefObjPtr(lua_State* L, int Index)
{
    int64 ObjID = FC_GetArgObjID(L, Index);
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    return ObjRef;
}

int64 FC_GetArgObjID(lua_State* L, int Index)
{
    int Type = lua_type(L, Index);
    int64 ObjID = 0;
    if (LUA_TUSERDATA == Type)
    {
        int64* Addr = (int64*)lua_touserdata(L, Index);
        if (Addr)
        {
            ObjID = *Addr;
        }
    }
    else if (LUA_TLIGHTUSERDATA == Type)
    {
        ObjID = (int64)lua_touserdata(L, Index);
    }
    else if (LUA_TTABLE == Type)
    {
        lua_pushstring(L, "__ObjectRefID");
        lua_rawget(L, Index);
        //lua_getfield(L, Index, "__ObjectRefID");
        ObjID = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    return ObjID;
}

UObject* FC_GetArgValue_Object(lua_State* L, int Index)
{
    FCObjRef* ObjRef = (FCObjRef *)FC_GetArgRefObjPtr(L, Index);
    if(ObjRef && ObjRef->IsValid())
    {
        FCObjRef* Parent = ObjRef->Parent;
        UObject* ThisObject = ObjRef->GetUObject();
        if (Parent && !ThisObject)
        {
            ThisObject = Parent->GetUObject();
        }
        return ThisObject;
    }
    return nullptr;
}

void FC_SetArgValue_Object(lua_State* L, const UObject* Object)
{
    if (!Object)
    {
        lua_pushnil(L);
        return;
    }
    int64 ObjID = FCGetObj::GetIns()->PushUObject((UObject *)Object);
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    if(ObjRef)
        FC_PushBindLuaValue(L, ObjID, ObjRef->ClassDesc->m_UEClassName.c_str());
    else
        FC_PushBindLuaValue(L, ObjID, "UObject");
}

void FC_PushBindLuaValue(lua_State* L, int64 ObjID, const char* ClassName)
{
    FCRefObjCache::GetIns()->PushBindLuaValue(L, ObjID, ClassName);

    //if (0)
    //{
    //    // 如果这个类型还没有注册过，再注册一次    
    //    GlbRegisterClass(L, ClassName);

    //    // 这里每次创建会有点慢，可以加一个全局缓存，缓存到lua里面，然后定时释放吧
    //    void* addr = lua_newuserdata(L, sizeof(int64*));
    //    *((int64*)addr) = ObjID;

    //    luaL_getmetatable(L, ClassName);
    //    lua_setmetatable(L, -2);
    //}
}