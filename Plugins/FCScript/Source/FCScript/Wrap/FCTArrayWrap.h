#pragma once
#include "FCBrigeHelper.h"

class FCTArrayWrap
{
public:	
    static void Register(lua_State* L);
	static int LibOpen_wrap(lua_State* L);

	static int obj_Index(lua_State* L);
	static int obj_NewIndex(lua_State* L);
	static int obj_new(lua_State* L);
	static int GetNumb_wrap(lua_State* L);
	static int SetNumb_wrap(lua_State* L);
	static int GetAt_wrap(lua_State* L);
	static int SetAt_wrap(lua_State* L);
	static int GetIndex_wrap(lua_State* L);
	static int SetIndex_wrap(lua_State* L);
	static int Swap_wrap(lua_State* L);
	static int Contains_wrap(lua_State* L);
	static int Add_wrap(lua_State* L);
	static int AddUnique_wrap(lua_State* L);
	static int Find_wrap(lua_State* L);
	static int Insert_wrap(lua_State* L);
	static int RemoveItem_wrap(lua_State* L);
	static int Remove_wrap(lua_State* L);
	static int Clear_wrap(lua_State* L);
	static int Reserve_wrap(lua_State* L);
	static int ToList_wrap(lua_State* L);
	static int SetList_wrap(lua_State* L);
};
