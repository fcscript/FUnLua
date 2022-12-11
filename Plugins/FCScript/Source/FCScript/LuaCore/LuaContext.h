#pragma once
//#include "CoreMinimal.h"
#include "FCDynamicClassDesc.h"

void SetTableForClass(lua_State* L, const char* Name);
uint8 CalcUserdataPadding(int32 Alignment);
bool TryToSetMetatable(lua_State* L, const char* MetatableName);
void SetUserdataPadding(lua_State* L, int32 Index, uint8 PaddingBytes);
void* NewUserdataWithPadding(lua_State* L, int32 Size, const char* MetatableName, uint8 PaddingSize);

FLuaValue   CloneLuaTable(lua_State* L, int SrcIdx);
int LoopTable(lua_State* L, int TableIdx, void (*Callback)(lua_State* L, int Index, void* UserData), void* UserData);

int ReportLuaCallError(lua_State* L);
int Global_Require(lua_State* L);
int Global_LoadFile(lua_State* L);
int Global_Print(lua_State* L);