#include "FCDynamicDelegateManager.h"
#include "FCGetObj.h"
#include "Misc/EngineVersionComparison.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "FCCallScriptFunc.h"
#include "FCDynamicOverrideFunc.h"
#include "FCLuaFunction.h"
#include "FCTemplateType.h"
#include "FCGetArg.h"
#include "../LuaCore/LuaContext.h"

FCDynamicDelegateManager & FCDynamicDelegateManager::GetIns()
{
    static FCDynamicDelegateManager sIns;
    return sIns;
}

FCLuaDelegate* FCDynamicDelegateManager::MakeLuaDelegate(UObject* Object, UObject* Outer, lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty)
{
    int Type = lua_type(L, ValueIdx);
    if (LUA_TFUNCTION != Type)
    {
        return nullptr;
    }
    const void* LuaFuncAddr = lua_topointer(L, ValueIdx);
    CAdr2DelegateMap::iterator itDelegate = m_FuncAdr2DelegateMap.find(LuaFuncAddr);
    if(itDelegate != m_FuncAdr2DelegateMap.end())
    {
        return itDelegate->second;
    }
    FCLuaDelegate  *Delegate = new FCLuaDelegate();
    Delegate->FunctionAddr = LuaFuncAddr;

    lua_pushvalue(L, ValueIdx);
    int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
    Delegate->FunctionRef = CallbackRef;

    // 生成UE的反射函数
    if(!Outer)
        Outer = GetScriptContext()->m_DelegateObject;
    Delegate->Object = Object;
    Delegate->Outer = Outer;
    MakeScriptDelegate(Delegate, LuaFuncAddr, Outer, DynamicProperty);

    Delegate->DynamicProperty = GetDynamicProperty(DynamicProperty->Property);
    Delegate->DynamicFunc = GetDynamicFunction(Delegate->Function);

    AddBindLuaFunction(Delegate, Object, LuaFuncAddr);
    return Delegate;
}
FCLuaDelegate* FCDynamicDelegateManager::MakeDelegateByTableParam(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty)
{
    // { self, func } 这种形式
    lua_rawgeti(L, ValueIdx, 1);
    int FirstType = lua_type(L, -1);
    lua_rawgeti(L, ValueIdx, 2);
    int SecondType = lua_type(L, -1);
    if (LUA_TTABLE == FirstType && LUA_TFUNCTION == SecondType)
    {
        FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, -2);
        const void* LuaFuncAddr = lua_topointer(L, -1);
        const void* LuaTableAddr = lua_topointer(L, -2);

        // 这个地方感觉不应该用函数地址限制，因为可能存在一对多的情况
        CAdr2DelegateMap::iterator itDelegate = m_FuncAdr2DelegateMap.find(LuaFuncAddr);
        if (itDelegate != m_FuncAdr2DelegateMap.end())
        {
            lua_pop(L, 2);
            if(itDelegate->second->ParamsValudAddr[0] != LuaTableAddr)
            {
                UE_DEBUG_BREAK();
                int iiii = 0;
            }
            return itDelegate->second;
        }
        UObject* Outer = GetScriptContext()->m_DelegateObject;
        UObject* Object = Outer;
        if (ObjRef && ObjRef->IsValid())
        {
            Object = ObjRef->GetUObject();
            if (!Object)
                Object = Outer;
            Outer = Outer ? Outer : Object;
        }
        if(!Outer)
        {
            return nullptr;
        }
        FCLuaDelegate* Delegate = new FCLuaDelegate();
        Delegate->FunctionAddr = LuaFuncAddr;       

        int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
        Delegate->FunctionRef = CallbackRef;
        Delegate->ParamCount = 1;
        Delegate->CallbackParams[0] = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
        Delegate->ParamsValudAddr[0] = LuaTableAddr;

        Delegate->Object = Object;
        Delegate->Outer = Outer;
        MakeScriptDelegate(Delegate, LuaFuncAddr, Outer, DynamicProperty);

        Delegate->DynamicProperty = GetDynamicProperty(DynamicProperty->Property);
        Delegate->DynamicFunc = GetDynamicFunction(Delegate->Function);

        AddBindLuaFunction(Delegate, Object, LuaFuncAddr);
        return Delegate;
    }
    else
    {
        lua_pop(L, 2);
        ReportLuaError(L, "invalid delegate param");
    }
    return nullptr;
}

FCLuaDelegate* FCDynamicDelegateManager::MakeDelegate(lua_State* L, const FCDelegateInfo* InDelegateInfo, const FCDynamicPropertyBase* DynamicProperty)
{
    if(!InDelegateInfo)
    {
        return nullptr;
    }

    return nullptr;
}

UObject* FCDynamicDelegateManager::OverridenLuaFunction(UObject* Object, UObject* Outer, lua_State* L, int ScriptIns, UFunction* ActionFunc, const FName& FuncName, bool bNoneCallByZeroParam)
{
    FString  NameValue(FuncName.ToString());
    const char *NewFuncName = TCHAR_TO_UTF8(*NameValue);
    if(!Outer)
        Outer = GetScriptContext()->m_DelegateObject;

    lua_rawgeti(L, LUA_REGISTRYINDEX, ScriptIns);
    lua_pushstring(L, NewFuncName);
    lua_rawget(L, -2);

    int Type = lua_type(L, -1);
    const void* LuaFuncAddr = lua_topointer(L, -1);
    if(LUA_TFUNCTION != Type)
    {
        lua_pop(L, 2);
        return nullptr;
    }
    CAdr2DelegateMap::iterator itDelegate = m_FuncAdr2DelegateMap.find(LuaFuncAddr);
    if (itDelegate != m_FuncAdr2DelegateMap.end())
    {
        lua_pop(L, 2);
        return Outer;
    }
    lua_pushvalue(L, -1);
    int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
    lua_pop(L, 2);

    FCLuaDelegate* Delegate = new FCLuaDelegate();
    Delegate->FunctionAddr = LuaFuncAddr;
    Delegate->FunctionRef = CallbackRef;
    Delegate->ScriptIns = ScriptIns;
    Delegate->bNoneCallByZeroParam = bNoneCallByZeroParam;

    // 生成UE的反射函数
    Delegate->Object = Object;
    Delegate->Outer = Outer;

    UClass* OuterClass = Outer->GetClass();
    Delegate->Function = MakeReplaceFunction(ActionFunc, OuterClass, FuncName, FCDynamicDelegate_CallLua);
    Delegate->DynamicFunc = GetDynamicFunction(Delegate->Function);
    Delegate->OuterClass = OuterClass;

    AddBindLuaFunction(Delegate, Object, LuaFuncAddr);
    return Outer;
}

void   FCDynamicDelegateManager::DeleteLuaDelegateByFuncAddr(const void* LuaFuncAddr)
{
    CAdr2DelegateMap::iterator itDelegate = m_FuncAdr2DelegateMap.find(LuaFuncAddr);
    if (itDelegate != m_FuncAdr2DelegateMap.end())
    {
        FCLuaDelegate* Delegate = itDelegate->second;

        EraseDelegatePtr(m_Object2ChildMap, Delegate, Delegate->Object);
        EraseFuncAddr2Delegete(Delegate, Delegate->Function);

        FCLuaDelegate* FindPtr = nullptr;
        if(Delegate->FunctionAddr == LuaFuncAddr)
        {
            FindPtr = Delegate;
            m_FuncAdr2DelegateMap.erase(itDelegate);
            if(Delegate->m_pNext)
            {
                m_FuncAdr2DelegateMap[LuaFuncAddr] = Delegate->m_pNext;
                Delegate->m_pNext = nullptr;
            }
        }
        else
        {
            FCLuaDelegate* LastPtr = nullptr;
            while (Delegate)
            {
                if (Delegate->FunctionAddr == LuaFuncAddr)
                {
                    FindPtr = Delegate;
                    if(LastPtr)
                    {
                        LastPtr->m_pNext = Delegate;
                    }
                    FindPtr->m_pNext = nullptr;
                }
                LastPtr = Delegate;
                Delegate = Delegate->m_pNext;
            }
        }

        if(FindPtr)
        {
            DeleteLuaDelegate(FindPtr);
        }
    }
}

void   FCDynamicDelegateManager::DeleteAllDelegateByUObject(const UObjectBase* Object)
{
    CAdr2DelegateMap::iterator itChild = m_Object2ChildMap.find(Object);
    if(itChild != m_Object2ChildMap.end())
    {
        FCLuaDelegate * Delegate = itChild->second;
        m_Object2ChildMap.erase(itChild);
        DeleteDelegateList(Delegate);
    }
}

void   FCDynamicDelegateManager::Clear()
{
    while(m_FuncAdr2DelegateMap.size() > 0)
    {
        CAdr2DelegateMap::iterator itBegin = m_FuncAdr2DelegateMap.begin();
        FCLuaDelegate* Delegate = itBegin->second;
        m_FuncAdr2DelegateMap.erase(itBegin);
        DeleteLuaDelegate(Delegate);
    }
    m_Object2ChildMap.clear();
    m_UEFuncAddr2DelegateMap.clear();

    ReleasePtrMap(m_DynamicFuncMap);
    ReleasePtrMap(m_DynamicProperyMap);
}

FCDynamicOverrideFunction* FCDynamicDelegateManager::GetDynamicFunction(UFunction* Function)
{
    CAdr2DynamicFuncMap::iterator itFunc = m_DynamicFuncMap.find(Function);
    if(itFunc != m_DynamicFuncMap.end())
    {
        return itFunc->second;
    }
    FCDynamicOverrideFunction *DynamicFunc = new FCDynamicOverrideFunction();
    DynamicFunc->InitParam(Function);
    m_DynamicFuncMap[Function] = DynamicFunc;
    return DynamicFunc;
}

void  FCDynamicDelegateManager::AddBindLuaFunction(FCLuaDelegate* Delegate, UObject* Object, const void* LuaFuncAddr)
{
    if (Object)
    {
        CAdr2DelegateMap::iterator itChild = m_Object2ChildMap.find(Object);
        if (itChild == m_Object2ChildMap.end())
        {
            m_Object2ChildMap[Object] = Delegate;
        }
        else
        {
            Delegate->m_pNext = itChild->second;
            m_Object2ChildMap[Object] = Delegate;
        }
        FCObjectUseFlag::GetIns().Ref(Object);
    }
    m_FuncAdr2DelegateMap[LuaFuncAddr] = Delegate;
    CAdr2DelegateMap::iterator itDelegate = m_UEFuncAddr2DelegateMap.find(Delegate->Function);
    if(itDelegate == m_UEFuncAddr2DelegateMap.end())
        m_UEFuncAddr2DelegateMap[Delegate->Function] = Delegate;
    else
    {
        itDelegate->second->m_pNextDelegate = Delegate;
    }
}

FCDynamicProperty* FCDynamicDelegateManager::GetDynamicProperty(const FProperty* InProperty, const char* InName)
{
    CAdr2DynamicPropertyMap::iterator itFind = m_DynamicProperyMap.find(InProperty);
    if(itFind != m_DynamicProperyMap.end())
    {
        return itFind->second;
    }
    FCDynamicProperty *DynamicPropery = new FCDynamicProperty();
    DynamicPropery->InitProperty(InProperty, InName);
    m_DynamicProperyMap[InProperty] = DynamicPropery;
    return DynamicPropery;
}

void  FCDynamicDelegateManager::EraseDelegatePtr(CAdr2DelegateMap& PtrMap, FCLuaDelegate* Delegate, const void* Key)
{
    if(!Key)
        return ;
    CAdr2DelegateMap::iterator itFind = PtrMap.find(Key);
    if(itFind != PtrMap.end())
    {
        if(itFind->second == Delegate)
        {
            if (Delegate->m_pNext)
            {
                PtrMap[Key] = Delegate->m_pNext;
            }
            else
            {
                PtrMap.erase(itFind);
            }
        }
        else
        {
            FCLuaDelegate* DelegateList = itFind->second;
            while (DelegateList)
            {
                if (DelegateList->m_pNext == Delegate)
                {
                    DelegateList->m_pNext = Delegate->m_pNext;
                    break;
                }
                DelegateList = DelegateList->m_pNext;
            }
        }
    }
}

void  FCDynamicDelegateManager::EraseFuncAddr2Delegete(FCLuaDelegate* Delegate, const void* Key)
{
    if(!Key)
        return ;
    CAdr2DelegateMap::iterator itFind = m_UEFuncAddr2DelegateMap.find(Key);
    if (itFind != m_UEFuncAddr2DelegateMap.end())
    {
        if(itFind->second == Delegate)
        {
            if(Delegate->m_pNextDelegate)
            {
                m_UEFuncAddr2DelegateMap[Key] = Delegate->m_pNextDelegate;
            }
            else
            {
                m_UEFuncAddr2DelegateMap.erase(itFind);
            }
        }
        else
        {
            FCLuaDelegate *DelegateList = itFind->second;
            while(DelegateList)
            {
                if(DelegateList->m_pNextDelegate == Delegate)
                {
                    DelegateList->m_pNextDelegate = Delegate->m_pNextDelegate;
                    break;
                }
                DelegateList = DelegateList->m_pNextDelegate;
            }
        }
    }
}

void  FCDynamicDelegateManager::DeleteDelegateList(FCLuaDelegate* Delegate)
{
    while(Delegate)
    {
        FCLuaDelegate *Ptr = Delegate;
        EraseFuncAddr2Delegete(Delegate, Delegate->Function);
        EraseDelegatePtr(m_FuncAdr2DelegateMap, Delegate, Delegate->FunctionAddr);

        Delegate = Delegate->m_pNext;
        DeleteLuaDelegate(Ptr);
    }
}

void  FCDynamicDelegateManager::DeleteLuaDelegate(FCLuaDelegate* Delegate)
{
    lua_State* L = GetScriptContext()->m_LuaState;
    if(L)
    {
        // 释放lua的引入变量
        for (int iParam = 0; iParam < Delegate->ParamCount; ++iParam)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, Delegate->CallbackParams[iParam]);
        }
        luaL_unref(L, LUA_REGISTRYINDEX, Delegate->FunctionRef);
    }

    Delegate->OuterClass->RemoveFunctionFromFunctionMap(Delegate->Function);

    ObjRefKey Key(nullptr, Delegate);
    FCObjRef *ObjRef = FCGetObj::GetIns()->FindObjRefByKey(Key);
    if(ObjRef)
    {
        ObjRef->Ref = 1;
        FCGetObj::GetIns()->DeleteValue(ObjRef->PtrIndex);
    }
    delete Delegate;
}

void FCDynamicDelegateManager::MakeScriptDelegate(FCLuaDelegate* Delegate, const void* LuaFuncAddr, UObject* Outer, const FCDynamicPropertyBase* DynamicProperty)
{
    if(!Outer)
    {
        return ;
    }
    FString NewName = FString::Printf(TEXT("LuaDelegate_%p"), LuaFuncAddr);
    FName  FuncName(*NewName);

    UClass* OuterClass = Outer->GetClass();
    FDelegateProperty  *DelegateProperty = (FDelegateProperty *)DynamicProperty->Property;
    UFunction* LuaFunction = MakeReplaceFunction(DelegateProperty->SignatureFunction, OuterClass, FuncName, FCDynamicDelegate_CallLua);

    Delegate->Delegate.BindUFunction(Outer, FuncName);
    Delegate->Function = LuaFunction;
    Delegate->OuterClass = OuterClass;
};

UFunction* FCDynamicDelegateManager::MakeReplaceFunction(UFunction* SrcFunction, UClass* OuterClass, const FName& NewFuncName, FNativeFuncPtr InFunc)
{
    UFunction* LuaFunction = OuterClass->FindFunctionByName(NewFuncName);
    if(!LuaFunction)
    {
        FObjectDuplicationParameters DuplicationParams(SrcFunction, OuterClass);
        DuplicationParams.InternalFlagMask &= ~EInternalObjectFlags::Native;
        DuplicationParams.DestName = NewFuncName;
        DuplicationParams.DestClass = UFCLuaFunction::StaticClass();
        LuaFunction = static_cast<UFunction*>(StaticDuplicateObjectEx(DuplicationParams));
        LuaFunction->FunctionFlags |= FUNC_Native;
        LuaFunction->ClearInternalFlags(EInternalObjectFlags::Native);
        LuaFunction->SetNativeFunc(InFunc);

        LuaFunction->SetSuperStruct(SrcFunction->GetSuperStruct());

        if (!FPlatformProperties::RequiresCookedData())
            UMetaData::CopyMetadata(SrcFunction, LuaFunction);

        LuaFunction->StaticLink(true);

        OuterClass->AddFunctionToFunctionMap(LuaFunction, NewFuncName);
    }

    // 如果当前函数与来源的函数参数不一样，修复一下
    if (LuaFunction->NumParms != SrcFunction->NumParms)
    {
        LuaFunction->NumParms = SrcFunction->NumParms;
        LuaFunction->ParmsSize = SrcFunction->ParmsSize;
        LuaFunction->ReturnValueOffset = SrcFunction->ReturnValueOffset;
        LuaFunction->RPCId = SrcFunction->RPCId;
        LuaFunction->RPCResponseId = SrcFunction->RPCResponseId;
    }
    return LuaFunction;
}

void FCDynamicDelegate_CallLua(UObject* Context, FFrame& TheStack, RESULT_DECL)
{
    // 先检查一下有没有绑定的对象
    UObject* Object = TheStack.Object;
    // 再检查一下，没有当前函数
    FCScriptContext* ScriptContext = GetScriptContext();
    if (ScriptContext)
    {
        UClass* Class = Object->GetClass();
        bool bUnpackParams = false;
        UFunction* Func = FirstNative(Context, TheStack, bUnpackParams);
        FCLuaDelegate* Delegate = FCDynamicDelegateManager::GetIns().FindDelegateByFunction(Func);
        bool bInitParam = false;
        bool bZeroParam = false;

        while(Delegate)
        {
            if(Delegate->bNoneCallByZeroParam)
            {
                if(!bInitParam)
                {
                    bInitParam = true;
                    if (Delegate->DynamicFunc->m_Property.size() == 1)
                    {
                        FCDynamicFunctionParam* DynamicProperty = Delegate->DynamicFunc->m_Property.data();
                        float* ParamValue = (float*)(TheStack.Locals + DynamicProperty->Offset_Internal);
                        if (*ParamValue == 0)
                        {
                            bZeroParam = true;
                        }
                    }
                }
                if (!bZeroParam)
                {
                    FCCallScriptDelegate(ScriptContext, Delegate->Object, Delegate->ScriptIns, *Delegate, Delegate->DynamicFunc, TheStack);
                }
            }
            else
            {
                FCCallScriptDelegate(ScriptContext, Delegate->Object, Delegate->ScriptIns, *Delegate, Delegate->DynamicFunc, TheStack);
            }

            Delegate = Delegate->m_pNextDelegate;
        }
    }
}