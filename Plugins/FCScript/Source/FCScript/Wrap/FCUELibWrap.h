#pragma once
#include "FCBrigeHelper.h"

// UnLua����


class FCUELibWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);
};