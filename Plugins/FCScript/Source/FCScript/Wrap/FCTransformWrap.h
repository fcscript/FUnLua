#pragma once
#include "FCBrigeHelper.h"

class FCTransformWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);

    static int Blend_wrap(lua_State* L);
    static int Inverse_wrap(lua_State* L);
    static int TransformPosition_wrap(lua_State* L);
    static int TransformPositionNoScale_wrap(lua_State* L);
    static int InverseTransformPosition_wrap(lua_State* L);
    static int InverseTransformPositionNoScale_wrap(lua_State* L);
    static int TransformVector_wrap(lua_State* L);
    static int TransformVectorNoScale_wrap(lua_State* L);
    static int InverseTransformVector_wrap(lua_State* L);
    static int InverseTransformVectorNoScale_wrap(lua_State* L);
    static int TransformRotation_wrap(lua_State* L);
    static int InverseTransformRotation_wrap(lua_State* L);

    static int tostring_wrap(lua_State* L);
    static int obj_New(lua_State* L);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
};
