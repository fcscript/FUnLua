#pragma once
#include "FCBrigeHelper.h"

class FCUObjectWrap
{
public:
    static void Register(lua_State* L);

    static int LibOpen_wrap(lua_State* L);

	static int obj_Index(lua_State* L);
	static int obj_NewIndex(lua_State* L);
	static int GetName_wrap(lua_State* L);
	static int GetOuter_wrap(lua_State* L);
	static int GetClass_wrap(lua_State* L);
	static int FindClass_wrap(lua_State* L);
	static int GetWorld_wrap(lua_State* L);
	static int GetMainGameInstance_wrap(lua_State* L);
	static int AddToRoot_wrap(lua_State* L);
	static int RemoveFromRoot_wrap(lua_State* L);
};