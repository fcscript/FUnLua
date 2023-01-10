#pragma once
#include "FCBrigeHelper.h"

class FCTSetWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int pair_gc(lua_State* L);
    static int obj_NextPairs(lua_State* L);
    static int obj_pairs(lua_State* L);
    static int obj_new(lua_State* L);
    static int GetNumb_wrap(lua_State* L);
    static int Contains_wrap(lua_State* L);

    static int Add_wrap(lua_State* L);
    static int Remove_wrap(lua_State* L);
    static int Clear_wrap(lua_State* L);
    static int ToArray_wrap(lua_State* L);
    static int SetArray_wrap(lua_State* L);

    static int GetMaxIndex_wrap(lua_State* L);
    static int ToNextValidIndex_wrap(lua_State* L);
    static int IsValidIndex_wrap(lua_State* L);
    static int GetAt_wrap(lua_State* L);
    static int RemoveAt_wrap(lua_State* L);
};
