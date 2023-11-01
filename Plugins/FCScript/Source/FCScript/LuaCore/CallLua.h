#pragma once
#include "CoreMinimal.h"
#include "FCDynamicClassDesc.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Containers/UnrealString.h"
#include "LuaHeader.h"
#include "FCSetArg.h"

template <typename T> struct TFLRemoveReference { typedef T Type; };
template <typename T> struct TFLRemoveReference<T& > { typedef T Type; };
template <typename T> struct TFLRemoveReference<T&&> { typedef T Type; };

template <typename T>
FORCEINLINE T&& LuaForward(typename TFLRemoveReference<T>::Type& Obj)
{
    return (T&&)Obj;
}

template <typename T>
FORCEINLINE T&& LuaForward(typename TFLRemoveReference<T>::Type&& Obj)
{
    return (T&&)Obj;
}

int  LuaPushValue(lua_State* L, bool value, bool bCopy);
int  LuaPushValue(lua_State* L, int8 value, bool bCopy);
int  LuaPushValue(lua_State* L, int16 value, bool bCopy);
int  LuaPushValue(lua_State* L, int32 value, bool bCopy);
int  LuaPushValue(lua_State* L, int64 value, bool bCopy);

int  LuaPushValue(lua_State* L, uint8 value, bool bCopy);
int  LuaPushValue(lua_State* L, uint16 value, bool bCopy);
int  LuaPushValue(lua_State* L, uint32 value, bool bCopy);
int  LuaPushValue(lua_State* L, uint64 value, bool bCopy);
int  LuaPushValue(lua_State* L, float value, bool bCopy);
int  LuaPushValue(lua_State* L, double value, bool bCopy);
int  LuaPushValue(lua_State* L, const char* value, bool bCopy);
int  LuaPushValue(lua_State* L, const FString& value, bool bCopy);
int  LuaPushValue(lua_State* L, const FName& value, bool bCopy);
int  LuaPushValue(lua_State* L, const FVector& value, bool bCopy);
int  LuaPushValue(lua_State* L, const FVector2D& value, bool bCopy);
int  LuaPushValue(lua_State* L, const FVector4& value, bool bCopy);
int  LuaPushValue(lua_State* L, const UObject* value, bool bCopy);
int  LuaPushValue(lua_State* L, const FLuaValue& value, bool bCopy);
int  LuaPushValue(lua_State* L, const FLuaTableRef& value, bool bCopy);

template <class _Ty>
int  LuaPushValue(lua_State* L, const TArray<_Ty> &Array, bool bCopy)
{
    FCScript::SetArgValue(L, Array);
    return 1;
}

template<class _TyKey, class _TyValue>
int LuaPushValue(lua_State* L, const TMap<_TyKey, _TyValue>& Map, bool bCopy)
{
    FCScript::SetArgValue(L, Map);
    return 1;
}

template <class _Ty>
int  LuaPushValue(lua_State* L, const TSet<_Ty>& Set, bool bCopy)
{
    FCScript::SetArgValue(L, Set);
    return 1;
}

UObject *LuaGetUObject(lua_State* L, int Idx);
int ReportLuaCallError(lua_State* L);


template<bool bCopy>
int  PushLuaArgs(lua_State* L)
{
    return false;
}

template <bool bCopy, typename T1, typename... T2>
int32 PushLuaArgs(lua_State* L, T1&& V1, T2&&... V2)
{
    return LuaPushValue(L, LuaForward<T1>(V1), bCopy) + PushLuaArgs<bCopy>(L, LuaForward<T2>(V2)...);
}

template <typename... T>
FLuaRetValues CallGlobalLua(lua_State* L, const char* FuncName, T&&... Args)
{
    int nStackValueNumb = lua_gettop(L); // 取当前栈内元素个数
    //lua_pushcfunction(L, ReportLuaCallError);
    lua_getglobal(L, FuncName);
    if (lua_isfunction(L, -1) == false)
    {
        lua_pop(L, 1);
        return FLuaRetValues(); // INDEX_NONE
    }
    int32 MessageHandlerIdx = lua_gettop(L) - 1;
    int32 NumArgs = PushLuaArgs<false>(L, Forward<T>(Args)...);
    int32 Code = lua_pcall(L, NumArgs, LUA_MULTRET, MessageHandlerIdx);
    // 说明：lua_pcall 调用后，会自动输入的相关参数(函数名+函数参数)
    int32 TopIdx = lua_gettop(L);

    if (Code == LUA_OK)
    {
        int32 NumResults = TopIdx - MessageHandlerIdx;
        return FLuaRetValues(TopIdx, NumResults);
    }

    int nRealPushParam = TopIdx - MessageHandlerIdx + 1;
    if (nRealPushParam > 0)
    {
        lua_pop(L, nRealPushParam);
    }
    return FLuaRetValues();
}

template <typename... T>
bool CallGlobalVoidLua(lua_State* L, const char* FuncName, T&&... Args)
{
    int nStackValueNumb = lua_gettop(L); // 取当前栈内元素个数
    //lua_pushcfunction(L, ReportLuaCallError);
    lua_getglobal(L, FuncName);
    if (lua_isfunction(L, -1) == false)
    {
        lua_pop(L, 1);
        return false;
    }
    int32 MessageHandlerIdx = lua_gettop(L) - 1;
    int32 NumArgs = PushLuaArgs<false>(L, Forward<T>(Args)...);
    int32 Code = lua_pcall(L, NumArgs, LUA_MULTRET, MessageHandlerIdx);
    // 说明：lua_pcall 调用后，会自动输入的相关参数(函数名+函数参数)
    int32 TopIdx = lua_gettop(L);
    int nRealPushParam = TopIdx - MessageHandlerIdx + 1;
    lua_pop(L, nRealPushParam);
    return Code == LUA_OK;
}

bool RawGetLuaFucntionByScriptIns(lua_State* L, int ScriptIns, const char* InFuncName);
bool RawGetLuaFunctionByTable(lua_State* L, int TableIdx, const char* InFuncName);
int ScriptCallInterface(lua_State* L, int ScriptIns, const char* FuncName, int ParamStartIdx, int ParamsCount);

// 调用Table的成员函数，有返回值的那种
template <typename... T>
FLuaRetValues CallTableFunction(lua_State* L, int TableIdx, const char* FuncName, T&&... Args)
{
    if (lua_type(L, TableIdx) != LUA_TTABLE)
    {
        return FLuaRetValues();
    }
    int StartIdx = lua_gettop(L);
    lua_pushcfunction(L, ReportLuaCallError);

    RawGetLuaFunctionByTable(L, TableIdx, FuncName);

    //lua_pushstring(L, FuncName);
    //lua_gettable(L, -2);

    int32 MessageHandlerIdx = lua_gettop(L) - 1;
    lua_pushvalue(L, TableIdx); // push self
    int32 NumArgs = PushLuaArgs<false>(L, Forward<T>(Args)...);
    int32 Code = lua_pcall(L, NumArgs + 1, LUA_MULTRET, MessageHandlerIdx);
    // 说明：lua_pcall 调用后，会自动输入的相关参数(函数名+函数参数)
    int32 TopIdx = lua_gettop(L);

    if (Code == LUA_OK)
    {
        int32 NumResults = TopIdx - MessageHandlerIdx;
        return FLuaRetValues(TopIdx, NumResults);
    }
    int32 CurIdx = lua_gettop(L);
    if (CurIdx > StartIdx)
    {
        lua_pop(L, CurIdx - StartIdx);
    }

    return FLuaRetValues();
}

// 调用Table的成员函数，无返回值的那种
template <typename... T>
bool CallTableVoidFunction(lua_State* L, int TableIdx, const char* FuncName, T&&... Args)
{
    if (lua_type(L, TableIdx) != LUA_TTABLE)
    {
        return false;
    }
    int StartIdx = lua_gettop(L);
    lua_pushcfunction(L, ReportLuaCallError);

    RawGetLuaFunctionByTable(L, TableIdx, FuncName);
    //lua_pushstring(L, FuncName);
    //lua_gettable(L, -2);

    bool bFunction = lua_isfunction(L, -1);
    if(!bFunction)
    {
        lua_pop(L, lua_gettop(L) - StartIdx);
        return false;
    }

    int32 MessageHandlerIdx = lua_gettop(L) - 1;
    lua_pushvalue(L, TableIdx); // push self
    int32 NumArgs = PushLuaArgs<false>(L, Forward<T>(Args)...);
    int32 Code = lua_pcall(L, NumArgs + 1, LUA_MULTRET, MessageHandlerIdx);
    //if(Code != LUA_OK)
    //{
    //    ReportLuaCallError(L);
    //}
    // 说明：lua_pcall 调用后，会自动输入的相关参数(函数名+函数参数)
    int32 CurIdx = lua_gettop(L);
    if (CurIdx > StartIdx)
    {
        lua_pop(L, CurIdx - StartIdx);
    }
    return Code == LUA_OK;
}
