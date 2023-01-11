#pragma once
#include "FCBrigeHelper.h"

class FCDataTableWrap
{
public:
    static void Register(lua_State* L);

    static int DoLoad_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetRow_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetRowNumb_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int pair_gc(lua_State* L);
    static int obj_NextPairs(lua_State* L);
    static int obj_pairs(lua_State* L);
    static int obj_Index(lua_State* L);
    static int GetNumb_Wrap(lua_State* L);
};
