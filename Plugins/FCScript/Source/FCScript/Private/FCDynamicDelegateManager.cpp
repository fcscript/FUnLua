#include "FCDynamicDelegateManager.h"
#include "FCGetObj.h"
#include "Misc/EngineVersionComparison.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "FCCallScriptFunc.h"
#include "FCDynamicOverrideFunc.h"
#include "FCLuaFunction.h"
#include "FCTemplateType.h"

FCDynamicDelegateManager & FCDynamicDelegateManager::GetIns()
{
    static FCDynamicDelegateManager sIns;
    return sIns;
}

int64  FCDynamicDelegateManager::MakeLuaDelegateRefObj(UObject* Object, lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty)
{
    FCLuaDelegate *Deleagte = MakeLuaDelegate(Object, L, ValueIdx, DynamicProperty);
    if(Deleagte)
    {
        if(0 == Deleagte->ObjRefID)
        {
            Deleagte->ObjRefID = FCGetObj::GetIns()->PushLuaDelegate(Deleagte);
            return Deleagte->ObjRefID;
        }
    }
    return 0;
}

FCLuaDelegate* FCDynamicDelegateManager::MakeLuaDelegate(UObject* Object, lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty)
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
    UObject *Outer = Object ? Object : GetScriptContext()->m_DelegateObject;
    Delegate->Object = Object;
    Delegate->Outer = Outer;
    MakeScriptDelegate(Delegate, LuaFuncAddr, Outer, DynamicProperty);

    Delegate->DynamicProperty = GetDynamicProperty(DynamicProperty->Property);
    Delegate->DynamicFunc = GetDynamicFunction(Delegate->Function);

    if(Object)
    {
        CAdr2DelegateMap::iterator itChild = m_Object2ChildMap.find(Object);
        if(itChild == m_Object2ChildMap.end())
        {
            m_Object2ChildMap[Object] = Delegate;
        }
        else
        {
            Delegate->m_pNext = itChild->second;
            m_Object2ChildMap[Object] = Delegate;
        }
    }
    m_FuncAdr2DelegateMap[LuaFuncAddr] = Delegate;
    m_UEFuncAddr2DelegateMap[Delegate->Function] = Delegate;
    return Delegate;
}

void   FCDynamicDelegateManager::DeleteLuaDelegateByFuncAddr(const void* LuaFuncAddr)
{
    CAdr2DelegateMap::iterator itDelegate = m_FuncAdr2DelegateMap.find(LuaFuncAddr);
    if (itDelegate != m_FuncAdr2DelegateMap.end())
    {
        FCLuaDelegate* Delegate = itDelegate->second;

        EraseDelegatePtr(m_Object2ChildMap, Delegate, Delegate->Object);
        EraseDelegatePtr(m_UEFuncAddr2DelegateMap, Delegate, Delegate->Function);

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
        DeleteDelegateList(Delegate);
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
        if(Delegate->m_pNext)
        {
            itFind->second = Delegate->m_pNext;
        }
        else
        {
            PtrMap.erase(itFind);
        }
    }
}

void  FCDynamicDelegateManager::DeleteDelegateList(FCLuaDelegate* Delegate)
{
    while(Delegate)
    {
        FCLuaDelegate *Ptr = Delegate;
        EraseDelegatePtr(m_UEFuncAddr2DelegateMap, Delegate, Delegate->Function);
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
    UFunction *Function = DelegateProperty->SignatureFunction;

    FObjectDuplicationParameters DuplicationParams(Function, OuterClass);
    DuplicationParams.InternalFlagMask &= ~EInternalObjectFlags::Native;
    DuplicationParams.DestName = FuncName;
    DuplicationParams.DestClass = UFCLuaFunction::StaticClass();
    DuplicationParams.ApplyFlags |= RF_ClassDefaultObject;
    UFunction *LuaFunction = static_cast<UFunction*>(StaticDuplicateObjectEx(DuplicationParams));
    LuaFunction->FunctionFlags |= FUNC_Native;
    LuaFunction->ClearInternalFlags(EInternalObjectFlags::Native);
    LuaFunction->SetNativeFunc(FCDynamicDelegate_CallLua);

    LuaFunction->SetSuperStruct(Function->GetSuperStruct());

    if (!FPlatformProperties::RequiresCookedData())
        UMetaData::CopyMetadata(Function, LuaFunction);

    LuaFunction->StaticLink(true);

    OuterClass->AddFunctionToFunctionMap(LuaFunction, FuncName);

    Delegate->Delegate.BindUFunction(Outer, FuncName);
    Delegate->Function = LuaFunction;
    Delegate->OuterClass = OuterClass;
};

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
        if (!Delegate)
        {
            return;
        }
        Object = Delegate->Object;
        FCCallScriptDelegate(ScriptContext, Object, Delegate->ScriptIns, *Delegate, Delegate->DynamicFunc, TheStack);
    }
}