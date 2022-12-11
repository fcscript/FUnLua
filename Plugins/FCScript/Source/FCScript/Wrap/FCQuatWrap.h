#pragma once
#include "FCBrigeHelper.h"

class FCQuatWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int Set_wrap(lua_State* L);

    static int GetNormalized_wrap(lua_State* L);
    static int ToAxisAndAngle_wrap(lua_State* L);
    static int Inverse_wrap(lua_State* L);
    static int RotateVector_wrap(lua_State* L);
    static int UnrotateVector_wrap(lua_State* L);

    static int GetAxisX_wrap(lua_State* L);
    static int GetAxisY_wrap(lua_State* L);
    static int GetAxisZ_wrap(lua_State* L);

    static int GetForwardVector_wrap(lua_State* L);
    static int GetRightVector_wrap(lua_State* L);
    static int GetUpVector_wrap(lua_State* L);

    static int Slerp_wrap(lua_State* L);
    static int ToEuler_wrap(lua_State* L);
    static int ToRotator_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
