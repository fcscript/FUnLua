#pragma once
#include "FCBrigeHelper.h"

class FCClassWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int DoLoad_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int New_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoIsChildOf_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetDefaultObject_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
};