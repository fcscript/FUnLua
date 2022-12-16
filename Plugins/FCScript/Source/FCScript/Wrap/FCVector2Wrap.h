#pragma once
#include "FCBrigeHelper.h"

class FCVector2Wrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int IsNormalized_wrap(lua_State* L);
    static int Cross_wrap(lua_State* L);

    static int double_add_wrap(lua_State* L);
    static int double_sub_wrap(lua_State* L);
    static int double_mul_wrap(lua_State* L);
    static int double_div_wrap(lua_State* L);
    static int num_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
