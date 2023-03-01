#include "FCUnLuaLib.h"
#include "FCDynamicClassDesc.h"
#include "FCRunTimeRegister.h"
#include "../LuaCore/CallLua.h"
#include "../LuaCore/LuaContext.h"

void FCUnLuaWrap::Register(lua_State* L)
{
    RegisterGlobalIndex(L);
    luaL_requiref(L, "UnLua", LibOpen_wrap, 1);
    //luaL_requiref(L, "FUnLua", LibOpen_wrap, 1);

    lua_register(L, "Log", LogInfo_wrap);
    lua_register(L, "LogWarn", LogWarn_wrap);
    lua_register(L, "LogError", LogError_wrap);
    lua_register(L, "FUnLua_Ref", Unref_wrap);
    lua_register(L, "FUnLua_Unref", Unref_wrap);
    lua_register(L, "FUnLua_Class", Unref_wrap);
}

int FCUnLuaWrap::LibOpen_wrap(lua_State* L)
{
    static constexpr LuaRegFunc LibFuncs[] = {
        {"Log", LogInfo_wrap},
        {"LogWarn", LogWarn_wrap},
        {"LogError", LogError_wrap},
        {"HotReload", HotReload_wrap},
        {"Ref", Ref_wrap},
        {"Unref", Unref_wrap},
        {"Class", Class_wrap},
        {NULL, NULL}
    };
    const char* ClassName = lua_tostring(L, 1);
    int Ret = FCExportedClass::RegisterLibClass(L, ClassName, (const LuaRegFunc*)LibFuncs);
    return Ret;
}

void FCUnLuaWrap::GetMessage(lua_State* L, FString& Message)
{
    const auto ArgCount = lua_gettop(L);
    if (!lua_checkstack(L, ArgCount))
    {
        luaL_error(L, "too many arguments, stack overflow");
        return ;
    }
    for (int ArgIndex = 1; ArgIndex <= ArgCount; ArgIndex++)
    {
        if (ArgIndex > 1)
            Message += "\t";
        Message += UTF8_TO_TCHAR(luaL_tolstring(L, ArgIndex, NULL));
    }
}

int FCUnLuaWrap::LogInfo_wrap(lua_State* L)
{
    FString Message;
    GetMessage(L, Message);
    UE_LOG(LogFCScript, Log, TEXT("%s"), *Message);
    return 0;
}

int FCUnLuaWrap::LogWarn_wrap(lua_State* L)
{
    FString Message;
    GetMessage(L, Message);
    UE_LOG(LogFCScript, Warning, TEXT("%s"), *Message);
    return 0;
}

int FCUnLuaWrap::LogError_wrap(lua_State* L)
{
    FString Message;
    GetMessage(L, Message);
    UE_LOG(LogFCScript, Error, TEXT("%s"), *Message);
    return 0;
}

int FCUnLuaWrap::HotReload_wrap(lua_State* L)
{
    return 0;
}

int FCUnLuaWrap::Ref_wrap(lua_State* L)
{
    UObject *Object = FCScript::GetUObject(L, 1);
    if (Object && GetScriptContext() && GetScriptContext()->m_ManualObjectReference)
        GetScriptContext()->m_ManualObjectReference->Add(Object);
    return 0;
}

int FCUnLuaWrap::Unref_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    if (Object && GetScriptContext() && GetScriptContext()->m_ManualObjectReference)
        GetScriptContext()->m_ManualObjectReference->Remove(Object);
    return 0;
}

int FCUnLuaWrap::Class_wrap(lua_State* L)
{
    const char *super_name  = lua_tostring(L, 1);
    lua_createtable(L, 0, 0);
    int nTableIdx = lua_gettop(L);

    //lua_getglobal(L, LUA_GNAME);
    //lua_pushstring(L, "__index");
    //lua_rawget(L, -2);
    //lua_rawset(L, nTableIdx);

    //lua_getglobal(L, LUA_GNAME);
    //lua_pushstring(L, "__newindex");
    //lua_rawget(L, -2);
    //lua_rawset(L, nTableIdx);

    lua_pushstring(L, "__index");                           // 2  对不存在的索引(成员变量)访问时触发
    lua_pushcfunction(L, OtherScript_Index);                      // 3
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");                        // 2  对不存在的索引(成员变量)赋值时触发
    lua_pushcfunction(L, OtherScript_NewIndex);                   // 3
    lua_rawset(L, -3);

    lua_pushvalue(L, -1);                           // set metatable to self
    lua_setmetatable(L, -2);

    if(super_name)
    {
        FLuaRetValues SuperTable = CallGlobalLua(L, "require", super_name);
        int RetCount = SuperTable.size();
        if(RetCount > 1)
        {
            lua_pop(L, RetCount - 1);            
            RetCount = 1;
        }
        if(RetCount == 1)
        {
            int RetType = lua_type(L, SuperTable[0]);
            if(LUA_TTABLE == RetType)
            {
                FLuaValue CloneValue = CloneLuaTable(L, SuperTable[0]);
                if(CloneValue.bValid)
                {
                    lua_pushstring(L, "Super");             // 不要脚本层修改这个变量
                    lua_pushvalue(L, CloneValue.ValueIdx);
                    lua_rawset(L, nTableIdx);
                }
                else
                {
                    FCStringBuffer128 ErrorTips;
                    ErrorTips << "UnLua.Class.Super failed, failed clone parent class:" << super_name;
                    ReportLuaError(L, ErrorTips.GetString());
                }
                RetCount = 0;
            }
        }
        else
        {
            FCStringBuffer128 ErrorTips;
            ErrorTips << "UnLua.Class failed, invalid class path:" << super_name;
            ReportLuaError(L, ErrorTips.GetString());
        }
        if(RetCount != 0)
        {
            lua_pop(L, RetCount);
        }
    }
    int  CurTop = lua_gettop(L);
    if(CurTop > nTableIdx)
    {
        lua_pop(L, CurTop - nTableIdx);
    }
    return 1;
}

void FCUnLuaWrap::RegisterGlobalIndex(lua_State* L)
{
    static const char* Chunk = R"(
            local rawget = _G.rawget
            local rawset = _G.rawset
            local rawequal = _G.rawequal
            local type = _G.type
            local getmetatable = _G.getmetatable
            local require = _G.require

            local NotExist = {}

            local function Index(t, k)
                local mt = getmetatable(t)
                local super = mt
                while super do
                    local v = rawget(super, k)
                    if v ~= nil and not rawequal(v, NotExist) then
                        rawset(t, k, v)
                        return v
                    end
                    super = rawget(super, "Super")
                end

                local p = mt[k]
                if p ~= nil then
                    if type(p) == "userdata" then
                        return GetUProperty(t, p)
                    elseif type(p) == "function" then
                        rawset(t, k, p)
                    elseif rawequal(p, NotExist) then
                        return nil
                    end
                else
                    rawset(mt, k, NotExist)
                end

                return p
            end

            local function NewIndex(t, k, v)
                local mt = getmetatable(t)
                local p = mt[k]
                if type(p) == "userdata" then
                    return SetUProperty(t, p, v)
                end
                rawset(t, k, v)
            end

            local function Class(super_name)
                local super_class = nil
                if super_name ~= nil then
                    super_class = require(super_name)
                end

                local new_class = {}
                new_class.__index = Index
                new_class.__newindex = NewIndex
                new_class.Super = super_class

              return new_class
            end
            
	        local global_mt = {}
	        global_mt.__index = Global_Index
	        setmetatable(_G, global_mt)
            
            _G.Index = Index
            _G.NewIndex = NewIndex
            _G.Class = Class     
            )";

    luaL_dostring(L, Chunk);
}
