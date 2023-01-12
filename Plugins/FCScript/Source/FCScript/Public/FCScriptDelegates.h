#pragma once

#include "CoreMinimal.h"

#include "../LuaCore/LuaHeader.h"

class FCSCRIPT_API FCScriptDelegates
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLuaContextCreateWrap, lua_State*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLuaContextStart, lua_State*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLuaContextCleanup, lua_State*);
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FLuaFileLoader, const char*, TArray<uint8>&);
    DECLARE_DELEGATE_RetVal_OneParam(int32, FGenericLuaDelegate, lua_State*);

    static FOnLuaContextCreateWrap OnLuaStateCreateWrap;  // LUA虚拟机创建后，可以执行一些注册的事件wrap
    static FOnLuaContextStart   OnLuaStateStart; // 执行完wrap注册后，开始执行代码了
    static FOnLuaContextCleanup OnLuaContextCleanup; // 虚拟机释放逻辑
    static FLuaFileLoader LoadLuaFile;  // 加载文件的拦截接口
    static FGenericLuaDelegate ReportLuaCallError;
};