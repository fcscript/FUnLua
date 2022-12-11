#pragma once
#include "FCBrigeHelper.h"

class TWeakObjectPtrWrap
{
public:	
    static void Register(lua_State* L);
	static int LibOpen_wrap(lua_State* L);
	static int Reset_wrap(lua_State* L);
	static int IsValid_wrap(lua_State* L);
	static int Get_wrap(lua_State* L);
	static int Set_wrap(lua_State* L);
};