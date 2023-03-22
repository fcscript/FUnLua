#include "FCDynamicClassDesc.h"

// 注册所有的反射接口

int Class_Index(lua_State* L);
int Class_NewIndex(lua_State* L);
int Global_RegisterEnum(lua_State* L);
int Global_RegisterClass(lua_State* L);
void GlbRegisterEnum(lua_State* L, const char* ClassName);
bool GlbReigterClassEx(lua_State* L, FCDynamicClassDesc* ClassDesc, const char* ClassName);
bool GlbRegisterBindScriptMetatable(lua_State* L, FCDynamicClassDesc* ClassDesc, const char *ClassName);
int Global_Index(lua_State* L);
int BindScript_GetBPObject(lua_State* L);
int BindScript_Index(lua_State* L);
int OtherScript_Index(lua_State* L);
int BindScript_NewIndex(lua_State* L);
int OtherScript_NewIndex(lua_State* L);
int BindScript_Equal(lua_State* L);
int Global_GetUProperty(lua_State* L);
int Global_SetUProperty(lua_State* L);
int UObject_Identical(lua_State* L);
int UObject_Delete(lua_State* L);
int Class_StaticClass(lua_State* L);
int Class_Cast(lua_State* L);
int BindScript_Overridden(lua_State* L);
int BindScript_CallOverridenFunction(lua_State* L);

int Class_CallGetLibFunction(lua_State* L);
int Class_CallSetLibFunction(lua_State* L);

void SetTableForClass(lua_State* L, const char* Name);
void RunTimeRegisterScript(FCScriptContext *Context);
// 一个闭包调用
int  Class_CallFunction(lua_State* L);
int  Class_CallLatentFunction(lua_State* L);
int  Class_CallInterfaceFunction(lua_State* L);
int  WrapNativeCallFunction(lua_State* L, int ParamIndex, UObject *ThisObject, FCDynamicFunction  *DynamicFunc, uint8 *Buffer, int BufferSize, FNativeFuncPtr NativeFuncPtr);

const char* GetPropertyType(lua_State* L, int Idx);