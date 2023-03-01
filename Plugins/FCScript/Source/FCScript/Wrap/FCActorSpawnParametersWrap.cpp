#include "FCActorSpawnParametersWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCTemplateType.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFActorSpawnParametersWrapClass("FActorSpawnParameters");

DEFINED_CLASS_VALUE(FActorSpawnParameters, Name)
DEFINED_CLASS_VALUE(FActorSpawnParameters, Template)
DEFIND_CLASS_UOBJECT_VALUE(FActorSpawnParameters, Owner)
DEFIND_CLASS_UOBJECT_VALUE(FActorSpawnParameters, Instigator)
DEFINED_CLASS_VALUE(FActorSpawnParameters, OverrideLevel)
DEFINED_CLASS_VALUE(FActorSpawnParameters, SpawnCollisionHandlingOverride)
DEFINED_CLASS_BIT_VALUE(FActorSpawnParameters, bNoFail)
DEFINED_CLASS_BIT_VALUE(FActorSpawnParameters, bDeferConstruction)
DEFINED_CLASS_BIT_VALUE(FActorSpawnParameters, bAllowDuringConstructionScript)
#if WITH_EDITOR
DEFINED_CLASS_VALUE(FActorSpawnParameters, NameMode)
DEFINED_CLASS_VALUE(FActorSpawnParameters, ObjectFlags)
#endif

void FCActorSpawnParametersWrap::Register(lua_State* L)
{
    luaL_requiref(L, "FActorSpawnParameters", LibOpen_wrap, 1);
}

int FCActorSpawnParametersWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "IsRemoteOwned", IsRemoteOwned_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        CLASS_VALUE_FUNC(FActorSpawnParameters, Name),
        CLASS_VALUE_FUNC(FActorSpawnParameters, Template),
        CLASS_VALUE_FUNC(FActorSpawnParameters, Owner),
        CLASS_VALUE_FUNC(FActorSpawnParameters, Instigator),
        CLASS_VALUE_FUNC(FActorSpawnParameters, OverrideLevel),

        CLASS_VALUE_FUNC(FActorSpawnParameters, SpawnCollisionHandlingOverride),
        CLASS_BIT_VALUE_FUNC(FActorSpawnParameters, bNoFail),
        CLASS_BIT_VALUE_FUNC(FActorSpawnParameters, bDeferConstruction),
        CLASS_BIT_VALUE_FUNC(FActorSpawnParameters, bAllowDuringConstructionScript),

        #if WITH_EDITOR
        CLASS_VALUE_FUNC(FActorSpawnParameters, NameMode),
        CLASS_VALUE_FUNC(FActorSpawnParameters, ObjectFlags),
        #endif
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__gc", obj_Delete },
        { "__call", obj_new },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    const char* ClassName = lua_tostring(L, 1);
    GFActorSpawnParametersWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCActorSpawnParametersWrap::obj_new(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterCppClass("FActorSpawnParameters");
    FActorSpawnParameters  *Value = new FActorSpawnParameters();
    int64 ObjID = FCGetObj::GetIns()->PushCppStruct(ClassDesc, Value);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

int FCActorSpawnParametersWrap::obj_Delete(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if(ObjRef)
    {
        FActorSpawnParameters* Value = (FActorSpawnParameters *)ObjRef->GetPropertyAddr();
        delete Value;
        ObjRef->ThisObjAddr = nullptr;
        FCGetObj::GetIns()->DeleteValue(ObjRef->PtrIndex);
    }
    return 0;
}

int FCActorSpawnParametersWrap::obj_Index(lua_State* L)
{
    return GFActorSpawnParametersWrapClass.DoLibIndex(L);
}
int FCActorSpawnParametersWrap::obj_NewIndex(lua_State* L)
{
    return GFActorSpawnParametersWrapClass.DoLibNewIndex(L);
}

int FCActorSpawnParametersWrap::IsRemoteOwned_wrap(lua_State* L)
{
    FActorSpawnParameters* A = (FActorSpawnParameters*)VectorBase_GetAddr(L, 1, "FActorSpawnParameters");
    FCScript::SetArgValue(L, A ? A->IsRemoteOwned() : false);
    return 1;
}
