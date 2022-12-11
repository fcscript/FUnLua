#include "FCScriptDelegates.h"


FCScriptDelegates::FOnLuaContextCreateWrap FCScriptDelegates::OnLuaStateCreateWrap;  // LUA虚拟机创建后，可以执行一些注册的事件wrap
FCScriptDelegates::FOnLuaContextStart   FCScriptDelegates::OnLuaStateStart; // 执行完wrap注册后，开始执行代码了
FCScriptDelegates::FOnLuaContextCleanup FCScriptDelegates::OnLuaContextCleanup; // 虚拟机释放逻辑
FCScriptDelegates::FLuaFileLoader FCScriptDelegates::LoadLuaFile;  // 加载文件的拦截接口
FCScriptDelegates::FGenericLuaDelegate FCScriptDelegates::ReportLuaCallError;