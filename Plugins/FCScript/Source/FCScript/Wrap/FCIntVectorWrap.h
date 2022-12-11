#pragma once
#include "FCBrigeHelper.h"

class FCIntVectorWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int Set_wrap(lua_State* L);
    static int mul_wrap(lua_State* L);
    static int div_wrap(lua_State* L);
    static int double_mul_wrap(lua_State* L);
    static int double_div_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
