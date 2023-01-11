#pragma once
#include "FCBrigeHelper.h"

class TSoftObjectPtrWrap
{
public:	
    static void Register(lua_State* L);
	static int LibOpen_wrap(lua_State* L);
    static int obj_New(lua_State* L);
	static int LoadSynchronous_wrap(lua_State* L);
	static int GetAssetName_wrap(lua_State* L);
};

class TSoftClassPtrWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);
    static int obj_New(lua_State* L);
};