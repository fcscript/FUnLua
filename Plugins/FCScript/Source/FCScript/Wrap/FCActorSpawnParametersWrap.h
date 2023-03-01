#pragma once
#include "FCBrigeHelper.h"

class FCActorSpawnParametersWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);
    static int obj_new(lua_State* L);
    static int obj_Delete(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
    static int IsRemoteOwned_wrap(lua_State* L);
};
