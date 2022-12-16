#pragma once
#include "FCBrigeHelper.h"

class FCIntPointWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int Set_wrap(lua_State* L);
    static int Add_wrap(lua_State* L);
    static int Sub_wrap(lua_State* L);
    static int Mul_wrap(lua_State* L);
    static int Div_wrap(lua_State* L);

    static int double_add_wrap(lua_State* L);
    static int double_sub_wrap(lua_State* L);
    static int double_mul_wrap(lua_State* L);
    static int double_div_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
