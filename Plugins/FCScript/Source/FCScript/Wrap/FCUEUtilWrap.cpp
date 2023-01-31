#include "FCUEUtilWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "FCTemplateType.h"
#include "FCCore.h"
#include "FCVectorBaseWrap.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

void FCUEUtilWrap::Register(lua_State* L)
{
    //luaL_requiref(L, "UEUtil", LibOpen_wrap, 1);
    luaL_requiref(L, "Overridden", LibOverriden_wrap, 1);

    FCDynamicClassDesc *ClassDesc = GetScriptContext()->RegisterUClass("UObject");
    if (ClassDesc)
    {
        ClassDesc->RegisterWrapLibFunction("GetName", DoGetName_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetWorld", DoGetWorld_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetClass", DoGetClass_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetParent", DoGetParent_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetOuter", DoGetParent_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetBPObject", DoGetBPObject_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetObjectAddr", DoGetObjectAddr_wrap, nullptr);
        ClassDesc->RegisterWrapLibAttrib("__GameObjectAddr", DoGetObjectAddr_wrap, nullptr);
        ClassDesc->RegisterWrapLibAttrib("__ObjectRefID", DoGetObjectRefID_wrap, nullptr);
        ClassDesc->RegisterWrapLibAttrib("Overridden", DoOverridden_wrap, nullptr);
        //ClassDesc->RegisterWrapLibAttrib("ClassDesc", DoGetClassDesc_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("StaticClass", DoGetStaticClass_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("AddGCRef", DoAddGCRef_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("ReleaseGCRef", DoReleaseGCRef_wrap, nullptr);
    }
    ClassDesc = GetScriptContext()->RegisterUClass("UWorld");
    if(ClassDesc)
    {
        ClassDesc->RegisterWrapLibFunction("SpawnActor", DoSpawActor_wrap, nullptr);
    }

    lua_register(L, "NewObject", NewObject_wrap);
    lua_register(L, "SpawActor", SpawActor_wrap);
    lua_register(L, "SpawnActor", SpawActor_wrap);
    lua_register(L, "LoadObject", LoadObject_wrap);
    lua_register(L, "LoadClass", LoadClass_wrap);
    lua_register(L, "LoadUserWidget", LoadUserWidget_wrap);
    lua_register(L, "GetBindObjectCount", GetBindObjectCount_wrap);
    lua_register(L, "GetTotalObjRef", GetTotalObjRef_wrap);
    lua_register(L, "GetTotalIntPtr", GetTotalIntPtr_wrap);
    lua_register(L, "GetObjRefSize", GetObjRefSize_wrap);
    lua_register(L, "GetClassDescMemSize", GetClassDescMemSize_wrap);
    lua_register(L, "ListGCObjects", ListGCObjects_wrap);
    lua_register(L, "PrintGCObjects", PrintGCObjects_wrap);

    // 注册基础数据类型，兼容UnLua
    RegisterBaseType(L);
}

void FCUEUtilWrap::RegisterBaseType(lua_State* L)
{
    luaL_requiref(L, "bool", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "int8", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "uint8", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "int16", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "uint16", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "int", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "int32", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "uint32", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "int64", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "uint64", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "float", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "double", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "FString", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "FName", LibOpenBaseType_wrap, 1);
    luaL_requiref(L, "FText", LibOpenBaseType_wrap, 1);
}

int FCUEUtilWrap::LibOpen_wrap(lua_State* L)
{
    const luaL_Reg LibFuncs[] =
    {
        { "NewObject", NewObject_wrap },
        { "SpawActor", SpawActor_wrap },
        { "LoadUserWidget", LoadUserWidget_wrap },
        { "GetBindObjectCount", GetBindObjectCount_wrap },
        { "GetTotalObjRef", GetTotalObjRef_wrap },
        { "GetTotalIntPtr", GetTotalIntPtr_wrap },
        { "GetObjRefSize", GetObjRefSize_wrap },
        { nullptr, nullptr }
    };

    return FCExportedClass::RegisterLibClass(L, "UEUtil", (const LuaRegFunc*)LibFuncs);
}

int FCUEUtilWrap::LibOpenBaseType_wrap(lua_State* L)
{
    const luaL_Reg LibFuncs[] =
    {
        { nullptr, nullptr }
    };
    const char* ClassName = lua_tostring(L, 1);
    return FCExportedClass::RegisterLibClass(L, ClassName, (const LuaRegFunc*)LibFuncs);
}

int FCUEUtilWrap::LibOverriden_wrap(lua_State* L)
{
    //const char* ClassName = lua_tostring(L, 1);
    luaL_newmetatable(L, "Overridden");

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, Overriden_Index);
    lua_rawset(L, -3);

    lua_pushvalue(L, -1);               // set metatable to self
    lua_setmetatable(L, -2);
    return 1;
}

int FCUEUtilWrap::Overriden_Index(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->IsValid())
    {
        const char* FuncName = lua_tostring(L, 2);
        FCDynamicClassDesc* ClassDesc = ObjRef->ClassDesc;
        FCDynamicFunction* DynamicFunction = ClassDesc->FindFunctionByName(FuncName);
        if (DynamicFunction)
        {
            lua_pushlightuserdata(L, (void*)(ObjRef->PtrIndex));
            lua_pushstring(L, FuncName);
            lua_pushcclosure(L, BindScript_CallOverridenFunction, 2);        // closure
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetName_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef *)ObjRefPtr;
    if (ThisObject)
    {
        FString Name = ThisObject->GetName();
        lua_pushstring(L, TCHAR_TO_UTF8(*Name));
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetWorld_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ThisObject)
    {
        UWorld* World = ThisObject->GetWorld();
        FCScript::PushUObject(L, World);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetClass_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ThisObject)
    {
        FCScript::PushUObject(L, ThisObject->GetClass());
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetParent_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ThisObject)
    {
        FCScript::PushUObject(L, ThisObject->GetOuter());
        return 1;
    }
    lua_pushnil(L);
    return 1;

}

int FCUEUtilWrap::DoGetBPObject_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ObjRef && ObjRef->IsValid())
    {
        ObjRef->Ref++;
        FCScript::PushBindObjRef(L, ObjRef->PtrIndex, ObjRef->ClassDesc->m_UEClassName);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetObjectAddr_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ObjRef && ObjRef->IsValid())
    {
        lua_pushlightuserdata(L, ObjRef->ThisObjAddr);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetObjectRefID_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ObjRef && ObjRef->IsValid())
    {
        lua_pushinteger(L, ObjRef->PtrIndex);
    }
    else
    {
        lua_pushinteger(L, 0);
    }
    return 1;
}

int FCUEUtilWrap::DoOverridden_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    int64 ObjID = 0;
    if (ObjRef && ObjRef->IsValid())
    {
        ObjID = ObjRef->PtrIndex;
    }
    void* addr = lua_newuserdata(L, sizeof(int64*));
    *((int64*)addr) = ObjID;

    luaL_getmetatable(L, "Overridden");
    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);
    return 1;
}

int FCUEUtilWrap::DoGetClassDesc_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if(ObjRef && ObjRef->IsValid())
    {
        if(ObjRef->ClassDesc)
        {
            FCScript::SetArgValue(L, ObjRef->ClassDesc->m_Class);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoGetStaticClass_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ObjRef && ObjRef->IsValid())
    {
        if (ObjRef->ClassDesc)
        {
            FCScript::SetArgValue(L, ObjRef->ClassDesc->m_Class);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

int FCUEUtilWrap::DoAddGCRef_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ObjRef && ObjRef->IsValid())
    {
        GetScriptContext()->m_ManualObjectReference->Add(ObjRef->GetUObject());
    }
    return 0;
}

int FCUEUtilWrap::DoReleaseGCRef_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    FCObjRef* ObjRef = (FCObjRef*)ObjRefPtr;
    if (ObjRef && ObjRef->IsValid())
    {
        GetScriptContext()->m_ManualObjectReference->Remove(ObjRef->GetUObject());
    }
    return 0;
}

int FCUEUtilWrap::DoSpawActor_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    return SpawActor_wrap(L);
}

FCDynamicClassDesc *UEUtil_FindClassDesc(const char *ClassName)
{
    if(!ClassName)
        return nullptr;
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
    if (!ClassDesc)
    {
        std::string  InClassName(ClassName);
        int Index = InClassName.rfind('.');
        if (Index < 0)
        {
            Index = InClassName.rfind('/');
            if (Index >= 0)
            {
                std::string Name = InClassName.substr(Index + 1);
                InClassName += '.';
                InClassName += Name;
            }
        }
        int Len = InClassName.size();
        if (!(Len > 2 && InClassName[Len - 1] == 'C' && InClassName[Len - 2] == '_'))
        {
            InClassName += "_C";
        }
        ClassDesc = GetScriptContext()->RegisterUClass(InClassName.c_str());
    }
    return ClassDesc;
}

int FCUEUtilWrap::NewObject_wrap(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = nullptr;
    int Type = lua_type(L, 1);
    if(Type == LUA_TSTRING)
    {
        const char* UEClassName = lua_tostring(L, 1);
        ClassDesc = UEUtil_FindClassDesc(UEClassName);
    }
    else
    {
        UStruct *ObjStruct = FCScript::GetUStruct(L, 1);
        if(ObjStruct)
            ClassDesc = GetScriptContext()->RegisterUStruct(ObjStruct);
    }

    UObject* Arg2 = FCScript::GetUObject(L, 2);
    UObject* Outer = Arg2 ? Arg2 : GetTransientPackage();

    const char* ObjectName = lua_tostring(L, 3);
    const char* ScriptClassName = lua_tostring(L, 4);
	if(ClassDesc && ClassDesc->m_Class)
	{
		FName  Name(NAME_None);
		if(ObjectName && ObjectName[0] != 0)
		{
			Name = FName(ObjectName);
        }


        // 后创建对象
        UObject  *Obj = nullptr;
#if OLD_UE_ENGINE
        Obj = StaticConstructObject_Internal(ClassDesc->m_Class, Outer, Name);
#else
        FStaticConstructObjectParameters ObjParams(ClassDesc->m_Class);
        ObjParams.Outer = Outer;
        ObjParams.Name = Name;
        Obj = StaticConstructObject_Internal(ObjParams);
#endif

        if(ScriptClassName && ScriptClassName[0] != 0)
        {
            FFCObjectdManager::GetSingleIns()->CallBindScript(Obj, ScriptClassName);
        }
        // 增加引用GC计数吧, 这个其实可以PushUObject里面做
        GetScriptContext()->m_ManualObjectReference->Add(Obj);
        FCScript::PushUObject(L, Obj);
        return 1;
	}
    lua_pushnil(L);
	return 1;
}
int FCUEUtilWrap::SpawActor_wrap(lua_State* L)
{
    FTransform Transform;
    FActorSpawnParameters SpawnParameters;

    int32 NumParams = lua_gettop(L);
    UWorld* World = Cast<UWorld>(FCScript::GetUObject(L, 1));
    UClass* Class = Cast<UClass>(FCScript::GetUStruct(L, 2));
    if(NumParams > 2)
    {
        FTransform* Arg3 = (FTransform*)VectorBase_GetAddr(L, 3, "FTransform");
        if(Arg3)
        {
            Transform = *Arg3;
        }
    }
    if(NumParams > 3)
    {
        SpawnParameters.SpawnCollisionHandlingOverride = (ESpawnActorCollisionHandlingMethod)lua_tointeger(L, 4);
    }
    AActor* Owner = nullptr;
    if(NumParams > 4)
    {
        Owner = Cast<AActor>(FCScript::GetUObject(L, 5));
        if(World && Owner && Owner->GetWorld() != World)
        {
            Owner = nullptr;
        }
        SpawnParameters.Owner = Owner;
    }

    if (NumParams > 5)
    {
        AActor* Actor = Cast<AActor>(FCScript::GetUObject(L, 6));
        if (Actor)
        {
            APawn* Instigator = Cast<APawn>(Actor);
            if (!Instigator)
            {
                Instigator = Actor->GetInstigator();
            }
            SpawnParameters.Instigator = Instigator;
        }
    }
    if (NumParams > 8)
    {
        ULevel* Level = Cast<ULevel>(FCScript::GetUObject(L, 9));
        if (Level)
        {
            SpawnParameters.OverrideLevel = Level;
        }
    }
    if (NumParams > 9)
    {
        SpawnParameters.Name = FName(lua_tostring(L, 10));
    }
    if(!World || !Class)
    {
        lua_pushnil(L);
        return 1;
    }

    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUStruct(Class);
    const char* ModuleName = NumParams > 6 ? lua_tostring(L, 7) : nullptr;
    AActor* NewActor = World->SpawnActor(Class, &Transform, SpawnParameters);
    if(!NewActor)
    {
        lua_pushnil(L);
        return 1;
    }
    if (ModuleName && ModuleName[0] != 0)
    {
        FFCObjectdManager::GetSingleIns()->CallBindScript(NewActor, ModuleName);
    }
    // 增加引用GC计数吧, 这个其实可以PushUObject里面做
    GetScriptContext()->m_ManualObjectReference->Add(NewActor);
    FCScript::PushUObject(L, NewActor);

    return 1;
}

// UObject.Load("/Game/Core/Blueprints/AI/BehaviorTree_Enemy.BehaviorTree_Enemy")
int FCUEUtilWrap::LoadObject_wrap(lua_State* L)
{
    const char* ObjectName = lua_tostring(L, 1);
    if (!ObjectName)
    {
        return 0;
    }

    FString ObjectPath(ObjectName);
    int32 Index = INDEX_NONE;
    ObjectPath.FindChar(TCHAR('.'), Index);
    if (Index == INDEX_NONE)
    {
        ObjectPath.FindLastChar(TCHAR('/'), Index);
        if (Index != INDEX_NONE)
        {
            const FString Name = ObjectPath.Mid(Index + 1);
            ObjectPath += TCHAR('.');
            ObjectPath += Name;
        }
    }
    UObject* Object = LoadObject<UObject>(nullptr, *ObjectPath);
    // 增加引用GC计数吧, 这个其实可以PushUObject里面做
    GetScriptContext()->m_ManualObjectReference->Add(Object);
    FCScript::PushUObject(L, Object);
    return 1;
}

// UClass.Load("/Game/Core/Blueprints/AICharacter.AICharacter_C")
int FCUEUtilWrap::LoadClass_wrap(lua_State* L)
{
    const char* ClassName = lua_tostring(L, 1);
    if (!ClassName)
    {
        return 0;
    }
    FCDynamicClassDesc* ClassDesc = UEUtil_FindClassDesc(ClassName);
    FCScript::PushUObject(L, ClassDesc ? ClassDesc->m_Struct : nullptr);
    return 1;
}

int FCUEUtilWrap::LoadUserWidget_wrap(lua_State* L)
{
    // 加载一个蓝图对象 UClass *LoadUserWiget(UObject* WorldContextObject, const char *ClassName, APlayerController* OwningPlayer);
    UObject* WorldContextObject = FCScript::GetUObject(L, 1);
    UObject* OwningPlayer = FCScript::GetUObject(L, 3);
    const char* ClassName = lua_tostring(L, 2);  // ClassName 必须是带 _C 结尾的, 如 "/Game/CMShow/GitCMShow/Test/CMShowDomo/UMG/CMShowDemoUI.CMShowDemoUI_C"
    UStruct* uiAsset = FC_FindUEClass(ClassName);

	UUserWidget  *UserWidget = UWidgetBlueprintLibrary::Create(WorldContextObject, Cast<UWidgetBlueprintGeneratedClass>(uiAsset), Cast< APlayerController>(OwningPlayer));
    if (UserWidget)
    {
        FCScript::PushUObject(L, UserWidget);
    }
    else
    {
        lua_pushnil(L);
    }
	return 1;
}
int FCUEUtilWrap::GetBindObjectCount_wrap(lua_State* L)
{
    lua_pushinteger(L, FFCObjectdManager::GetSingleIns()->GetBindObjectCount());
    return 1;
}
int FCUEUtilWrap::GetTotalObjRef_wrap(lua_State* L)
{
    lua_pushinteger(L, FCGetObj::GetIns()->GetTotalObjRef());
    return 1;
}
int FCUEUtilWrap::GetTotalIntPtr_wrap(lua_State* L)
{
    lua_pushinteger(L, FCGetObj::GetIns()->GetTotalIntPtr());
    return 1;
}
int FCUEUtilWrap::GetObjRefSize_wrap(lua_State* L)
{
    lua_pushinteger(L, sizeof(FCObjRef));
    return 1;
}
int FCUEUtilWrap::GetClassDescMemSize_wrap(lua_State* L)
{
    const char *ClassName = lua_tostring(L, 1);
    if(ClassName)
    {
        FCDynamicClassDesc *DynamicClassDesc = GetScriptContext()->RegisterUClass(ClassName);
        lua_pushinteger(L, DynamicClassDesc ? DynamicClassDesc->GetMemSize() : 0);
    }
    else
    {
        lua_pushinteger(L, GetScriptContext()->GetMemSize());
    }
    return 1;
}

int FCUEUtilWrap::ListGCObjects_wrap(lua_State* L)
{
    const TSet<UObject*> &ObjList = GetScriptContext()->m_ManualObjectReference->GetReferencedObjects();
    TSet<UObject*>::TConstIterator it = ObjList.CreateConstIterator();
    lua_createtable(L, 0, 0);
    int nTableIdx = lua_gettop(L);
    int Index = 0;
    for(; it; ++it)
    {
        UObject *Obj = *it;
        if(Obj)
        {
            FCScript::PushUObject(L, Obj);
            lua_rawseti(L, nTableIdx, Index + 1);
            ++Index;
        }
    }
    int nCurIdx = lua_gettop(L);
    return 1;
}

int FCUEUtilWrap::PrintGCObjects_wrap(lua_State* L)
{
    const TSet<UObject*>& ObjList = GetScriptContext()->m_ManualObjectReference->GetReferencedObjects();
    TSet<UObject*>::TConstIterator it = ObjList.CreateConstIterator();
    UE_LOG(LogFCScript, Log, TEXT("Lua GC Objects total num:%d"), ObjList.Num());

    for (; it; ++it)
    {
        UObject* Obj = *it;
        if (Obj)
        {
            UE_LOG(LogFCScript, Log, TEXT("Lua GC Objects %p : %s"), Obj, *(Obj->GetName()));
        }
    }
    return 0;
}

