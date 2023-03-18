#include "FCDelegateWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"

void FCDelegateWrap::Register(lua_State* L)
{
    luaL_requiref(L, "DelegateEvent", RegisterDelegate, 1);
    luaL_requiref(L, "MulticastDelegateEvent", RegisterDelegate, 1);

    luaL_requiref(L, "FScriptDelegate", RegisterScriptDelegate, 1);
}

int FCDelegateWrap::RegisterDelegate(lua_State* L)
{
    //FCExportedClass::RegisterLibClass(L, ClassName, LibFuncs);
    const luaL_Reg LibFuncs[] =
    {
        { "AddListener", AddListener_wrap },
        { "Add", AddListener_wrap },
        { "Bind", AddListener_wrap },
        { "RemoveListener", RemoveListener_wrap },
        { "Remove", RemoveListener_wrap },
        { "Unbind", RemoveListener_wrap },
        { "ClearLinstener", ClearLinstener_wrap },
        { "Clear", ClearLinstener_wrap },
        { "Invoke", Invoke_wrap },
        { "Execute", Invoke_wrap },
        { "__gc", FCExportedClass::obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    int ParamCount = lua_gettop(L);
    const char* ClassName = lua_tostring(L, 1);

    luaL_newmetatable(L, ClassName);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, LibFuncs, 0);
    luaL_newlib(L, LibFuncs);
    return 1;
}

int FCDelegateWrap::RegisterScriptDelegate(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "__call", obj_new },
        { "__gc", obj_release },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };
    const char* ClassName = lua_tostring(L, 1);
    return FCExportedClass::RegisterLibClass(L, ClassName, LibFuncs);
}

int FCDelegateWrap::obj_new(lua_State* L)
{
    // DelegateEvent(self, func) 两个参数
    // DelegateEvent(func) 一个参数
    int ParamCount = lua_gettop(L);  // 获取当前参数数量
    const char *ClassName = lua_tostring(L, -1);

    FCDelegateInfo* DelegateInfo = new FCDelegateInfo();

    int FunctionRef = -1;
    bool bFindFunc = false;
    const void* FuncAddr = nullptr;
    int* CallbackParams = DelegateInfo->CallbackParams;
    int CallbackParamCount = 0;
    for (int i = 2; i <= ParamCount; ++i)
    {
        int nType = lua_type(L, i);
        lua_pushvalue(L, i);
        int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
        if (nType == LUA_TFUNCTION && !bFindFunc)
        {
            bFindFunc = true;
            FunctionRef = CallbackRef;
            FuncAddr = lua_topointer(L, i);
        }
        else if (CallbackParamCount < FCDelegateInfo::CallbackParamMax)
        {
            CallbackParams[CallbackParamCount++] = CallbackRef;
        }
    }
    DelegateInfo->ParamCount = CallbackParamCount;

    int64 ObjID = FCGetObj::GetIns()->PushLuaDelegate(DelegateInfo);
    FCScript::PushBindObjRef(L, ObjID, "FScriptDelegate");
    return 1;
}

int FCDelegateWrap::obj_release(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if(ObjRef && ObjRef->RefType == EFCObjRefType::LuaDelegate)
    {
        FCDelegateInfo* DelegateInfo = (FCDelegateInfo*)ObjRef->GetPropertyAddr();
        delete DelegateInfo;
        ObjRef->Ref = 1;
        FCGetObj::GetIns()->DeleteValue(ObjRef->PtrIndex);
    }    
    return 0;    
}

int FCDelegateWrap::AddListener_wrap(lua_State* L)
{
   // obj.onclick:AddListener(function)
   // obj.onclick:AddListener(function, user_param ...)
   // AddListener(obj.onclick, function)
   // AddListener(obj.onclick, function, user_param ...)
    int ParamCount = lua_gettop(L);  // 获取当前参数数量
    FCObjRef* ObjRef = (FCObjRef *)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && EFCObjRefType::RefProperty == ObjRef->RefType)
    {
        UObject* Object = ObjRef->GetParentObject();
        int FunctionRef = -1;
        bool bFindFunc = false;
        const void* FuncAddr = nullptr;
        int CallbackParams[FCDelegateInfo::CallbackParamMax];
        int CallbackParamCount = 0;
        for(int i = 2; i<= ParamCount; ++i)
        {
            int nType = lua_type(L, i);
            lua_pushvalue(L, i);
            int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
            if(nType == LUA_TFUNCTION && !bFindFunc)
            {
                bFindFunc = true;
                FunctionRef = CallbackRef;
                FuncAddr = lua_topointer(L, i);
            }
            else if(CallbackParamCount < FCDelegateInfo::CallbackParamMax)
            {
                CallbackParams[CallbackParamCount++] = CallbackRef;
            }
        }
        if(!bFindFunc)
        {
            return 0;
        }
        FFCObjectdManager::GetSingleIns()->RegisterScriptDelegate(Object, ObjRef->DynamicProperty, FuncAddr, FunctionRef, CallbackParams, CallbackParamCount, ObjRef->GetPropertyAddr());
    }
	return 0;
}

int FCDelegateWrap::RemoveListener_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    int nType = lua_type(L, 2);
	if(ObjRef && EFCObjRefType::RefProperty == ObjRef->RefType && nType == LUA_TFUNCTION)
	{
        const void * FuncAddr = lua_topointer(L, 2);
		UObject  *Object = ObjRef->GetParentObject();
		FFCObjectdManager::GetSingleIns()->RemoveScriptDelegate(Object, ObjRef->DynamicProperty, FuncAddr);
	}
	return 0;
}

int FCDelegateWrap::ClearLinstener_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if(ObjRef && EFCObjRefType::RefProperty == ObjRef->RefType)
	{
        UObject* Object = ObjRef->GetParentObject();
        FFCObjectdManager::GetSingleIns()->ClearScriptDelegate(Object, ObjRef->DynamicProperty);
	}
	return 0;
}

int FCDelegateWrap::Invoke_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if(ObjRef && EFCObjRefType::RefProperty == ObjRef->RefType)
	{
        UObject* Object = ObjRef->GetParentObject();
        // 暂时不支持 Invoke(...)
	}
	return 0;
}
