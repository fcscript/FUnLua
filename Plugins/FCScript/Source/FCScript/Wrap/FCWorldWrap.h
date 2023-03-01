#pragma once
#include "FCBrigeHelper.h"

class FCWorldWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int DoSpawnActorEx_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int GetTimeSeconds_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
};
