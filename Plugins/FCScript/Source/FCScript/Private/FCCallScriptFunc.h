#pragma once
#include "FCDynamicClassDesc.h"
#include "UObject/Class.h"
#include "../LuaCore/CallLua.h"

//---------------------------------------------------------------------------------
// 将UE变量Push到Lua栈中
void  PushScriptDefault(lua_State *L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptBool(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptInt8(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptInt16(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptInt32(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptInt64(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptFloat(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptDouble(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptFString(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptFName(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptFVector(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptFVector2D(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptFVector4D(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptStruct(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptUObject(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  PushScriptCppPtr(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr);
void  PushScriptMapIterator(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr);

LPPushScriptValueFunc  InitDynamicPropertyWriteFunc(FCPropertyType Flag);

//---------------------------------------------------------------------------------

void  ReadScriptDefault(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptBool(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptInt8(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptInt16(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptInt32(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptInt64(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptFloat(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptDouble(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptFString(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptFName(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptStruct(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptUObject(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
void  ReadScriptCppPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr);
void  ReadScriptMapIterator(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr);

void  InitDynamicPropertyReadFunc(FCDynamicProperty *DynamicProperty, FCPropertyType Flag);

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------

// C++调用脚本函数(不定参数, 没有返回值的)
template <typename... T>
bool  CallAnyScriptFunc(FCScriptContext* Context, int64 ScriptIns, const char *ScriptFuncName, T&&... Args)
{
	lua_State* L = Context->m_LuaState;
	if(L)
	{
        int StartTop = lua_gettop(L);
        lua_pushcfunction(L, ReportLuaCallError);
		lua_rawgeti(L, LUA_REGISTRYINDEX, ScriptIns);
		int TabldIdx = lua_gettop(L);
        bool bSuc = CallTableVoidFunction(L, TabldIdx, ScriptFuncName, Args...);
        int CurTop = lua_gettop(L);
        if(CurTop > StartTop)
        {
            lua_pop(L, CurTop - StartTop);
        }
        return bSuc;
	}
    return false;
}

bool  FCCallScriptFunc(FCScriptContext *Context, UObject *Object, int64 ScriptIns, const char *ScriptFuncName, FCDynamicFunction* DynamicFunction, FFrame& TheStack);
void  FCCallScriptDelegate(FCScriptContext *Context, UObject *Object, int64 ScriptIns, const FCDelegateInfo &DelegateInfo, FCDynamicFunction* DynamicFunction, FFrame& TheStack);
