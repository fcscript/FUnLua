#pragma once
#include "FCBrigeHelper.h"

class FCVector4Wrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int double_add_wrap(lua_State* L);
    static int double_sub_wrap(lua_State* L);
    static int double_mul_wrap(lua_State* L);
    static int double_div_wrap(lua_State* L);
    static int num_wrap(lua_State* L);

    static int Size3_Wrap(lua_State* L);
    static int SizeSquared3_Wrap(lua_State* L);
    static int Dot3_Wrap(lua_State* L);
    static int Dot4_Wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
