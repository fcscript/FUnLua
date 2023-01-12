#include "FCUObjectWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "FCCore.h"
#include "../LuaCore/LuaContext.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

void FCUObjectWrap::Register(lua_State* L)
{
	luaL_requiref(L, "UObject", LibOpen_wrap, 1);
}

int FCUObjectWrap::LibOpen_wrap(lua_State* L)
{
    const char* ClassName = lua_tostring(L, 1);

    const luaL_Reg LibFuncs[] =
    {
        { "GetName", GetName_wrap },
        { "GetOuter", GetOuter_wrap },
        { "GetClass", GetClass_wrap },
        { "FindClass", FindClass_wrap },
        { "GetWorld", GetWorld_wrap },
        { "GetMainGameInstance", GetMainGameInstance_wrap },
        { "AddToRoot", AddToRoot_wrap },
        { "RemoveFromRoot", RemoveFromRoot_wrap },

        { "__gc", UObject_Delete },
        { "__eq", UObject_Identical },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { nullptr, nullptr }
    };
    //luaL_newmetatable(L, ClassName);

    //lua_pushstring(L, "__index");                           // 2  对不存在的索引(成员变量)访问时触发
    //lua_pushcfunction(L, obj_Index);                      // 3
    //lua_rawset(L, -3);

    //lua_pushstring(L, "__newindex");                        // 2  对不存在的索引(成员变量)赋值时触发
    //lua_pushcfunction(L, obj_NewIndex);                   // 3
    //lua_rawset(L, -3);

    //luaL_setfuncs(L, LibFuncs, 0);
    //luaL_newlib(L, LibFuncs);

    FCExportedClass::RegisterLibClass(L, ClassName, (const LuaRegFunc*)LibFuncs);
	return 1;
}

int FCUObjectWrap::obj_Index(lua_State* L)
{
    return Class_Index(L);
}

int FCUObjectWrap::obj_NewIndex(lua_State* L)
{
    return Class_NewIndex(L);
}

int FCUObjectWrap::GetName_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    if (Object)
    {
        FString Name = Object->GetName();
        lua_pushstring(L, TCHAR_TO_UTF8(*Name));
        return 1;
    }
    lua_pushnil(L);
    return 1;
}
int FCUObjectWrap::GetOuter_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    FCScript::PushUObject(L, Object ? Object->GetOuter() : nullptr);
    return 1;
}
int FCUObjectWrap::GetClass_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    FCScript::PushUObject(L, Object ? Object->GetClass() : nullptr);
    return 0;
}
int FCUObjectWrap::FindClass_wrap(lua_State* L)
{
    const char* ClassName = lua_tostring(L, 1); // ClassName 必须是带 _C 结尾的, 如 "/Game/CMShow/GitCMShow/Test/CMShowDomo/UMG/CMShowDemoUI.CMShowDemoUI_C"

    UStruct* Struct = FC_FindUEClass(ClassName);
    FCScript::PushUObject(L, Struct);
    return 1;
}
int FCUObjectWrap::GetWorld_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    FCScript::PushUObject(L, Object ? Object->GetWorld() : nullptr);
    return 1;
}
int FCUObjectWrap::GetMainGameInstance_wrap(lua_State* L)
{
    UGameInstance* GameInstance = GEngine->GameViewport ? GEngine->GameViewport->GetGameInstance() : nullptr;
    FCScript::PushUObject(L, GameInstance);
    return 1;
}
int FCUObjectWrap::AddToRoot_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    if (Object)
    {
        Object->AddToRoot();
    }
    return 0;
}
int FCUObjectWrap::RemoveFromRoot_wrap(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    if (Object)
    {
        Object->RemoveFromRoot();
    }
    return 0;
}