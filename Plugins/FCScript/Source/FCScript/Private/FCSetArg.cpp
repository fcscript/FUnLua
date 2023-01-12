
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

void* FC_GetArgRefObjPtr(lua_State* L, int Index)
{
    int64 ObjID = FC_GetArgObjID(L, Index);
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    return ObjRef;
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

UStruct* FC_GetArgValue_UStruct(lua_State* L, int Index)
{
    int Type = lua_type(L, Index);
    if (LUA_TTABLE == Type)
    {
        // 直接取类名吧    
        //lua_pushvalue(L, Index);
        lua_pushstring(L, "__name");        // 3
        int ClassType = lua_rawget(L, Index);                 // 3
        const char* UETypeName = lua_tostring(L, -1);
        if(UETypeName)
        {
            FCDynamicClassDesc *DynamicClassDesc = GetScriptContext()->RegisterUClass(UETypeName);
            if(DynamicClassDesc)
                return DynamicClassDesc->m_Struct;
        }
    }
    else if(LUA_TSTRING == Type)
    {
        const char* UEClassName = lua_tostring(L, 1);
        if(UEClassName)
        {
            FCDynamicClassDesc* DynamicClassDesc = GetScriptContext()->RegisterUClass(UEClassName);
            if (DynamicClassDesc)
                return DynamicClassDesc->m_Struct;
        }
    }
    UObject *ArgObj = FC_GetArgValue_Object(L, Index);
    UStruct* ObjStruct = Cast<UStruct>(ArgObj);
    return ObjStruct;
}

UObject* FC_GetArgValue_UStructOrUObject(lua_State* L, int Index)
{
    int Type = lua_type(L, Index);
    if (LUA_TTABLE == Type)
    {
        // 直接取类名吧    
        //lua_pushvalue(L, Index);
        lua_pushstring(L, "__name");        // 3
        int ClassType = lua_rawget(L, Index);                 // 3
        const char* UETypeName = lua_tostring(L, -1);
        if (UETypeName)
        {
            FCDynamicClassDesc* DynamicClassDesc = GetScriptContext()->RegisterUClass(UETypeName);
            if (DynamicClassDesc)
                return DynamicClassDesc->m_Struct;
        }
    }
    else if (LUA_TSTRING == Type)
    {
        const char* UEClassName = lua_tostring(L, 1);
        if (UEClassName)
        {
            FCDynamicClassDesc* DynamicClassDesc = GetScriptContext()->RegisterUClass(UEClassName);
            if (DynamicClassDesc)
                return DynamicClassDesc->m_Struct;
        }
    }
    UObject* ArgObj = FC_GetArgValue_Object(L, Index);
    return ArgObj;
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
        FC_PushBindLuaValue(L, ObjID, ObjRef->ClassDesc->m_UEClassName);
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

void FC_PushArray(lua_State* L, const void* ArrayData, int ArrayLen, const char* InnerType)
{
    FCDynamicProperty* DynamicProperty = GetTArrayDynamicProperty(InnerType);
    if (DynamicProperty)
    {
        FScriptArray* ScriptArray = new FScriptArray();
        int64 ObjID = FCGetObj::GetIns()->PushTemplate(DynamicProperty, ScriptArray, EFCObjRefType::NewTArray);

        FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
        FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
        FProperty* Inner = ArrayProperty->Inner;
        int ElementSize = Inner->GetSize();

        void  FCTArrayWrap_SetNumb(FScriptArray * ScriptArray, FProperty * Inner, int NewNum);
        FCTArrayWrap_SetNumb(ScriptArray, Inner, ArrayLen);

        FMemory::Memcpy(ScriptArray->GetData(), ArrayData, ArrayLen * ElementSize);

        FC_PushBindLuaValue(L, ObjID, "TArray");
    }
    else
    {
        lua_pushnil(L);
    }
}
