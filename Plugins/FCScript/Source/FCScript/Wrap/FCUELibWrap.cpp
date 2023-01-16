#include "FCUELibWrap.h"
#include "FCRunTimeRegister.h"

void FCUELibWrap::Register(lua_State* L)
{
    lua_getglobal(L, LUA_GNAME);
    lua_setglobal(L, "UE");

    lua_getglobal(L, LUA_GNAME);
    lua_setglobal(L, "UE4");
}

int FCUELibWrap::LibOpen_wrap(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, Global_Index);
    lua_rawset(L, -3);

    lua_pushvalue(L, -1);
    lua_setmetatable(L, -2);

    lua_getglobal(L, "UE");
    lua_setglobal(L, "UE4");
    return 1;

    //const LuaRegFunc LibFuncs[] =
    //{
    //    { "__index", Global_Index },
    //    { nullptr, nullptr }
    //};
    //const char* ClassName = lua_tostring(L, 1);
    //return FCExportedClass::RegisterLibClass(L, ClassName, (const LuaRegFunc*)LibFuncs);
}