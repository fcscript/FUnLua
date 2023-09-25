#include "FCFunctionLibrary.h"

#include "FCDynamicClassDesc.h"
#include "FCObjectManager.h"
#include "FCCallScriptFunc.h"
#include "FCTemplateType.h"

void FunLib_SkipStepCompiledParam(FFrame& Stack)
{
    while (Stack.PeekCode() != EX_EndFunctionParms)
    {
        Stack.MostRecentPropertyAddress = nullptr;
        Stack.MostRecentProperty = nullptr;
        Stack.StepCompiledIn<FProperty>(nullptr);
    }
    P_FINISH
}

void FunLib_CallLuaByStack(lua_State* L, int32 StartIdx, int32 NumArgs, FFrame& Stack)
{
    int32 MessageHandlerIdx = lua_gettop(L) - NumArgs;

    while (Stack.PeekCode() != EX_EndFunctionParms)
    {
        Stack.MostRecentPropertyAddress = nullptr;
        Stack.MostRecentProperty = nullptr;
        Stack.StepCompiledIn<FProperty>(nullptr);
        FCDynamicProperty* DynamicProprety = GetDynamicPropertyByUEProperty(Stack.MostRecentProperty);
        if (DynamicProprety)
        {
            DynamicProprety->m_WriteScriptFunc(L, DynamicProprety, (uint8*)Stack.MostRecentPropertyAddress, nullptr, nullptr);
            ++NumArgs;
        }
    }
    P_FINISH
    
    int32 Code = lua_pcall(L, NumArgs, LUA_MULTRET, MessageHandlerIdx);
    int32 CurIdx = lua_gettop(L);
    if (CurIdx > StartIdx)
    {
        lua_pop(L, CurIdx - StartIdx);
    }
}

//DEFINE_FUNCTION(UFCFunctionLibrary::execCallLuaTableFunction)
void UFCFunctionLibrary::execCallLuaTableFunction(UObject* Context, FFrame& Stack, RESULT_DECL)
{
    P_GET_PROPERTY(FStrProperty, LuaFileName);
    P_GET_PROPERTY(FStrProperty, Function);
    P_GET_PROPERTY(FBoolProperty, SelfCall);

    FCScriptContext* ScriptContext = GetScriptContext();

    const char * Str_LuaFileName = TCHAR_TO_ANSI(*LuaFileName);
    const char* Str_Function = TCHAR_TO_ANSI(*Function);

    lua_State* L = ScriptContext->m_LuaState;
    int StartIdx = lua_gettop(L);

    lua_pushcfunction(L, ReportLuaCallError);
    FLuaRetValues  TableRetValue = CallGlobalLua(L, "require", Str_LuaFileName);
    lua_pushcfunction(L, ReportLuaCallError);
    int TableIdx = -1;
    if(TableRetValue.IsValid())
    {
        TableIdx = TableRetValue[0];
        bool bFindFunc = RawGetLuaFunctionByTable(L, TableIdx, Str_Function);
        if(!bFindFunc)
        {
            lua_pop(L, lua_gettop(L) - StartIdx);
            FunLib_SkipStepCompiledParam(Stack);
            return;            
        }
    }
    else
    {
        lua_getglobal(L, Str_Function);
        bool bFunction = lua_isfunction(L, -1);
        if (!bFunction)
        {
            lua_pop(L, lua_gettop(L) - StartIdx);
            FunLib_SkipStepCompiledParam(Stack);
            return;
        }
    }
    int32 NumArgs = 0;
    if(SelfCall)
    {
        NumArgs = 1;
        lua_pushvalue(L, TableIdx); // push self
    }
    FunLib_CallLuaByStack(L, StartIdx, NumArgs, Stack);
}

//DEFINE_FUNCTION(UFCFunctionLibrary::execCallLuaFunction)
void UFCFunctionLibrary::execCallLuaFunction(UObject* Context, FFrame& Stack, RESULT_DECL)
{
    P_GET_PROPERTY(FStrProperty, Function);
    const char* Str_Function = TCHAR_TO_ANSI(*Function);

    FCScriptContext* ScriptContext = GetScriptContext();
    lua_State* L = ScriptContext->m_LuaState;
    int StartIdx = lua_gettop(L);

    lua_pushcfunction(L, ReportLuaCallError);
    lua_getglobal(L, Str_Function);
    bool bFunction = lua_isfunction(L, -1);
    if (!bFunction)
    {
        lua_pop(L, lua_gettop(L) - StartIdx);
        FunLib_SkipStepCompiledParam(Stack);
        return;
    }
    FunLib_CallLuaByStack(L, StartIdx, 0, Stack);
}
