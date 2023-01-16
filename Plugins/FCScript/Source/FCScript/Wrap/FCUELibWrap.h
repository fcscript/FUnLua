#pragma once
#include "FCBrigeHelper.h"

// UnLuaºÊ»›


class FCUELibWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);
};