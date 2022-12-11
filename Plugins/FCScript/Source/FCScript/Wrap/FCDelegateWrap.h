#pragma once
#include "FCBrigeHelper.h"

class FCDelegateWrap
{
public:	
    static void Register(lua_State* L);
    static int RegisterDelegate(lua_State* L);
	static int AddListener_wrap(lua_State* L);
	static int RemoveListener_wrap(lua_State* L);
	static int ClearLinstener_wrap(lua_State* L);
	static int Invoke_wrap(lua_State* L);
};