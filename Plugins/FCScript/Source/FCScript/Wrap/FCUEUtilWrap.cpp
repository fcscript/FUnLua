#include "FCUEUtilWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "FCTemplateType.h"
#include "FCCore.h"

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
    }

    lua_register(L, "NewObject", NewObject_wrap);
    lua_register(L, "SpawActor", SpawActor_wrap);
    lua_register(L, "LoadObject", LoadObject_wrap);
    lua_register(L, "LoadClass", LoadClass_wrap);
    lua_register(L, "LoadUserWidget", LoadUserWidget_wrap);
    lua_register(L, "GetBindObjectCount", GetBindObjectCount_wrap);
    lua_register(L, "GetTotalObjRef", GetTotalObjRef_wrap);
    lua_register(L, "GetTotalIntPtr", GetTotalIntPtr_wrap);

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
        FCScript::PushBindObjRef(L, ObjRef->PtrIndex, ObjRef->ClassDesc->m_UEClassName.c_str());
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
        UObject* Arg1 = FCScript::GetUObject(L, 1);
        UStruct *ObjStruct = Cast<UStruct>(Arg1);
        ClassDesc = GetScriptContext()->RegisterUStruct(ObjStruct);
    }

    UObject* Arg2 = FCScript::GetUObject(L, 2);
    UObject* Outer = Arg2 ? Arg2 : GetTransientPackage();

    const char* ObjectName = lua_tostring(L, 3);
    const char* ScriptClassName = lua_tostring(L, 4);
	if(ClassDesc)
	{
		FName  Name(NAME_None);
		if(ObjectName && ObjectName[0] != 0)
		{
			Name = FName((const TCHAR*)ObjectName);
        }
        int64  ObjID = FCGetObj::GetIns()->PushNewObject(ClassDesc, Name, Outer);
        if(ScriptClassName && ScriptClassName[0] != 0)
        {
            UObject* Object = FCGetObj::GetIns()->GetUObject(ObjID);
            FFCObjectdManager::GetSingleIns()->CallBindScript(Object, ScriptClassName);
        }
        FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName.c_str());
        return 1;
	}
    lua_pushnil(L);
	return 1;
}
int FCUEUtilWrap::SpawActor_wrap(lua_State* L)
{
	return NewObject_wrap(L);
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
