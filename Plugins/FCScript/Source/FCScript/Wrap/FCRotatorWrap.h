#pragma once
#include "FCBrigeHelper.h"

class FCRotatorWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int Set_wrap(lua_State* L);

    static int GetRightVector_wrap(lua_State* L);
    static int GetUpVector_wrap(lua_State* L);
    static int GetUnitAxis_wrap(lua_State* L);
    static int RotateVector_wrap(lua_State* L);
    static int UnrotateVector_wrap(lua_State* L);
    static int Clamp_wrap(lua_State* L);

    static int GetForwardVector_wrap(lua_State* L);
    static int ToVector_wrap(lua_State* L);
    static int ToEuler_wrap(lua_State* L);
    static int ToQuat_wrap(lua_State* L);
    static int Inverse_wrap(lua_State* L);
    static int MakeFromEuler_wrap(lua_State* L);

    static int GetPitch_wrap(lua_State* L);
    static int GetYaw_wrap(lua_State* L);
    static int GetRoll_wrap(lua_State* L);

    static int SetPitch_wrap(lua_State* L);
    static int SetYaw_wrap(lua_State* L);
    static int SetRoll_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
