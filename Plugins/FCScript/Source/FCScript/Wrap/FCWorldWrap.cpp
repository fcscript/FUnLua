#include "FCWorldWrap.h"
#include "FCUEUtilWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "../LuaCore/LuaContext.h"
#include "FCVectorBaseWrap.h"

void FCWorldWrap::Register(lua_State* L)
{
    FCDynamicClassDesc *ClassDesc = GetScriptContext()->RegisterUClass("UWorld");
    if (ClassDesc)
    {
        ClassDesc->RegisterWrapLibFunction("SpawnActor", FCUEUtilWrap::DoSpawActor_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("SpawnActorEx", DoSpawnActorEx_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetTimeSeconds", GetTimeSeconds_wrap, nullptr);
    }

}

int FCWorldWrap::LibOpen_wrap(lua_State* L)
{
    return 0;
}

int FCWorldWrap::DoSpawnActorEx_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    int32 NumParams = lua_gettop(L);
    if (NumParams < 2)
        return ReportLuaError(L, "[UWorld::SpawnActorEx]invalid parameters");

    UWorld* World = Cast<UWorld>(FCScript::GetUObject(L, 1));
    if (!World)
        return ReportLuaError(L, "[UWorld::SpawnActorEx]invalid world");

    UClass* Class = Cast<UClass>(FCScript::GetUStruct(L, 2));
    if (!Class)
        return ReportLuaError(L, "[UWorld::SpawnActorEx]invalid class");

    FTransform Transform;
    if (NumParams > 2)
    {
        FTransform* Arg3 = (FTransform*)VectorBase_GetAddr(L, 3, "FTransform");
        if (Arg3)
        {
            Transform = *Arg3;
        }
    }
    int32 InitializerTableRef = LUA_NOREF;
    if (NumParams > 3 && lua_type(L, 4) == LUA_TTABLE)
    {
        lua_pushvalue(L, 4);
        InitializerTableRef = luaL_ref(L, LUA_REGISTRYINDEX);
        // Initialize函数的参数
    }
    const char* ModuleName = NumParams > 4 ? lua_tostring(L, 5) : nullptr;
    FActorSpawnParameters SpawnParameters;
    if (NumParams > 5)
    {
        FActorSpawnParameters* Arg6 = (FActorSpawnParameters*)VectorBase_GetAddr(L, 6, "FActorSpawnParameters");
        if (Arg6)
        {
            SpawnParameters = *Arg6;
        }
    }
    AActor* NewActor = World->SpawnActor(Class, &Transform, SpawnParameters);
    if (!NewActor)
    {    
        if (InitializerTableRef != LUA_NOREF)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, InitializerTableRef);
        }
        lua_pushnil(L);
        return 1;
    }
    if (ModuleName && ModuleName[0] != 0)
    {
        FFCObjectdManager::GetSingleIns()->CallBindScript(NewActor, ModuleName, InitializerTableRef);
    }
    else if(InitializerTableRef != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, InitializerTableRef);
    }
    // 增加引用GC计数吧, 这个其实可以PushUObject里面做
    GetScriptContext()->m_ManualObjectReference->Add(NewActor);
    FCScript::PushUObject(L, NewActor);

    return 1;
}

int FCWorldWrap::GetTimeSeconds_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    UWorld* World = Cast<UWorld>(FCScript::GetUObject(L, 1));
    FCScript::SetArgValue(L, World ? World->GetTimeSeconds() : 0.0f);
    return 1;
}
//
//BEGIN_EXPORT_CLASS(FActorSpawnParameters)
//ADD_PROPERTY(Name)
//ADD_PROPERTY(Template)
//ADD_PROPERTY(Owner)
//ADD_PROPERTY(Instigator)
//ADD_PROPERTY(OverrideLevel)
//#if WITH_EDITOR
//#if ENGINE_MAJOR_VERSION > 4 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 26)
//ADD_PROPERTY(OverridePackage)
//ADD_PROPERTY(OverrideParentComponent)
//ADD_PROPERTY(OverrideActorGuid)
//#endif
//#endif
//ADD_PROPERTY(SpawnCollisionHandlingOverride)
//ADD_FUNCTION(IsRemoteOwned)
//ADD_BITFIELD_BOOL_PROPERTY(bNoFail)
//ADD_BITFIELD_BOOL_PROPERTY(bDeferConstruction)
//ADD_BITFIELD_BOOL_PROPERTY(bAllowDuringConstructionScript)
//#if WITH_EDITOR
//ADD_BITFIELD_BOOL_PROPERTY(bTemporaryEditorActor)
//ADD_BITFIELD_BOOL_PROPERTY(bHideFromSceneOutliner)
//#if ENGINE_MAJOR_VERSION > 4 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 26)
//ADD_BITFIELD_BOOL_PROPERTY(bCreateActorPackage)
//#endif
//ADD_PROPERTY(NameMode)
//ADD_PROPERTY(ObjectFlags)
//#endif
//END_EXPORT_CLASS()
//
//IMPLEMENT_EXPORTED_CLASS(FActorSpawnParameters)
