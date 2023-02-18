#pragma once
#include "FCBrigeHelper.h"

class FCLinearColorWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int Set_wrap(lua_State* L);
    static int FromSRGBColor_wrap(lua_State* L);
    static int ToFColor_wrap(lua_State* L);
    static int Clamp_wrap(lua_State* L);

    static int GetR_wrap(lua_State* L);
    static int GetG_wrap(lua_State* L);
    static int GetB_wrap(lua_State* L);
    static int GetA_wrap(lua_State* L);

    static int SetR_wrap(lua_State* L);
    static int SetG_wrap(lua_State* L);
    static int SetB_wrap(lua_State* L);
    static int SetA_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
