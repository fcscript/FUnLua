#pragma once
//#include "CoreMinimal.h"
#include "FCDynamicClassDesc.h"

void SetTableForClass(lua_State* L, const char* Name);

FLuaValue   CloneLuaTable(lua_State* L, int SrcIdx);
int LoopTable(lua_State* L, int TableIdx, void (*Callback)(lua_State* L, int Index, void* UserData), void* UserData);

int ReportLuaCallError(lua_State* L);
int ReportLuaError(lua_State* L, const char *ErrorTips);
int Global_Require(lua_State* L);
int Global_LoadFile(lua_State* L);
int Global_Print(lua_State* L);
bool RawGetLuaFucntionByScriptIns(lua_State* L, int ScriptIns, const char *InFuncName);
bool RawGetLuaFunctionByTable(lua_State* L, int TableIdx, const char* InFuncName);