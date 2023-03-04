#pragma once
#include "FCBrigeHelper.h"

// UnLua兼容


class FCUnLuaWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);
    static void GetMessage(lua_State* L, FString & Message);
    static int LogInfo_wrap(lua_State* L);
    static int LogWarn_wrap(lua_State* L);
    static int LogError_wrap(lua_State* L);
    static int HotReload_wrap(lua_State* L);
    static int Ref_wrap(lua_State* L);
    static int Unref_wrap(lua_State* L);
    static int Class_wrap(lua_State* L);
    static int DebugBreak_wrap(lua_State* L);
    static void RegisterGlobalIndex(lua_State* L);
};