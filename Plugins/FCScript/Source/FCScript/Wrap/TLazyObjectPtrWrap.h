#pragma once
#include "FCBrigeHelper.h"

class TLazyObjectPtrWrap
{
public:	
    static void Register(lua_State* L);
	static int LibOpen_wrap(lua_State* L);
    static int obj_New(lua_State* L);
	static int Reset_wrap(lua_State* L);
	static int ResetWeakPtr_wrap(lua_State* L);
	static int IsValid_wrap(lua_State* L);
	static int IsPending_wrap(lua_State* L);
	static int Get_wrap(lua_State* L);
	static int Set_wrap(lua_State* L);
};