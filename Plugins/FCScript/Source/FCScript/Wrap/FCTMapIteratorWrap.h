#pragma once
#include "FCBrigeHelper.h"
#include "FCDynamicClassDesc.h"

class FCTMapIteratorWrap
{
public:	
    static void Register(lua_State* L);
	static int LibOpen_wrap(lua_State* L);

	static int obj_new(lua_State* L);

    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
	static int Key_wrap(lua_State* L);
	static int GetValue_wrap(lua_State* L);
	static int SetValue_wrap(lua_State* L);

	static int IsValid_wrap(lua_State* L);
	static int ToNext_wrap(lua_State* L);
	static int Reset_wrap(lua_State* L);

	static FScriptMap *GetScriptMap(int64 ObjID);

	static int32 ToNextValidIterator(FScriptMap* ScriptMap, int32 NextIndex);
};
