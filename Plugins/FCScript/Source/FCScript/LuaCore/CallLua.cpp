#include "CallLua.h"
#include "UObject/UnrealType.h"
#include "FCGetObj.h"
#include "FCTemplateType.h"
#include "FCGetArg.h"
#include "FCSetArg.h"


int  LuaPushValue(lua_State* L, bool value, bool bCopy)
{
    lua_pushboolean(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, int8 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, int16 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, int32 value, bool bCopy)
{
    lua_pushnumber(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, int64 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, uint8 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, uint16 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, uint32 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, uint64 value, bool bCopy)
{
    lua_pushinteger(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, float value, bool bCopy)
{
    lua_pushnumber(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, double value, bool bCopy)
{
    lua_pushnumber(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const char* value, bool bCopy)
{
    lua_pushstring(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const FString& value, bool bCopy)
{
    FCScript::SetArgValue(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const FName& value, bool bCopy)
{
    FCScript::SetArgValue(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const FVector& value, bool bCopy)
{
    FCScript::SetArgValue(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const FVector2D& value, bool bCopy)
{
    FCScript::SetArgValue(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const FVector4& value, bool bCopy)
{
    FCScript::SetArgValue(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const UObject* value, bool bCopy)
{
    FCScript::PushUObject(L, value);
    return 1;
}

int  LuaPushValue(lua_State* L, const FLuaValue& value, bool bCopy)
{
    if (value.bValid)
    {
        lua_pushvalue(L, value.ValueIdx);
        return 1;
    }
    return 0;
}

UObject* LuaGetUObject(lua_State* L, int Idx)
{
    return FCScript::GetUObject(L, Idx);
}