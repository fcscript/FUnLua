#pragma once
#include "FCBrigeHelper.h"

class FCColorWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int GetR_wrap(lua_State* L);
    static int GetG_wrap(lua_State* L);
    static int GetB_wrap(lua_State* L);
    static int GetA_wrap(lua_State* L);

    static int SetR_wrap(lua_State* L);
    static int SetG_wrap(lua_State* L);
    static int SetB_wrap(lua_State* L);
    static int SetA_wrap(lua_State* L);

    static int Set_wrap(lua_State* L);
    static int Add_wrap(lua_State* L);
    static int ToLinearColor_wrap(lua_State* L);

    static int double_add_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
