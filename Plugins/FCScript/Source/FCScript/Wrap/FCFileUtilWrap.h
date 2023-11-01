#pragma once
#include "FCBrigeHelper.h"

class FCFileUtilWrap
{
public:
    static void Register(lua_State* L);
    static int LibOpen_wrap(lua_State* L);
    static int GetAllFiles_wrap(lua_State* L);
    static int CopyFile_wrap(lua_State* L);
    static int DeleteFile_wrap(lua_State* L);
    static int ReadText_wrap(lua_State* L);
    static int WriteText_wrap(lua_State* L);
    static int FileExists_wrap(lua_State* L);
    static int DirectoryExists_wrap(lua_State* L);
};
