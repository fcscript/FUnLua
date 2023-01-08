#pragma once
#include "FCBrigeHelper.h"

class FCTMapWrap
{
public:	
    static void Register(lua_State* L);
	static int LibOpen_wrap(lua_State* L);

    static int pair_gc(lua_State* L);
    static int obj_NextPairs(lua_State* L);
    static int obj_pairs(lua_State* L);
	static int obj_new(lua_State* L);
	static int GetNumb_wrap(lua_State* L);
	static int GetIndex_wrap(lua_State* L);
	static int SetIndex_wrap(lua_State* L);
	static int Add_wrap(lua_State* L);
	static int Remove_wrap(lua_State* L);
	static int Clear_wrap(lua_State* L);
	static int ToMap_wrap(lua_State* L);
	static int SetMap_wrap(lua_State* L);
    static int begin_wrap(lua_State* L);
    static int find_wrap(lua_State* L);
};
