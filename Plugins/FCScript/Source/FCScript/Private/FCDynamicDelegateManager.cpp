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
    FCLuaDelegateKey Key(Object, LuaFuncAddr);
    CLuaAddr2DelegateMap::iterator itDelegate = m_LuaAddr2DelegateMap.find(Key);
    if(itDelegate != m_LuaAddr2DelegateMap.end())
    {
        return itDelegate->second;
    }

    FCLuaDelegate  *Delegate = new FCLuaDelegate();
    Delegate->FunctionAddr = LuaFuncAddr;
    Delegate->m_Key = Key;

    lua_pushvalue(L, ValueIdx);
    int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
    Delegate->FunctionRef = CallbackRef;

    // 生成UE的反射函数
    if(!Outer)
        Outer = GetScriptContext()->m_DelegateObject;
    Delegate->Object = Object;
    Delegate->Outer = Outer;
    MakeScriptDelegate(Delegate, LuaFuncAddr, Outer, DynamicProperty);

    Delegate->DynamicProperty = GetDynamicProperty(DynamicProperty->SafePropertyPtr->GetProperty());
    Delegate->DynamicFunc = GetDynamicFunction(Delegate->Function);

    AddBindLuaFunction(Delegate, Object);
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
        FCLuaDelegateKey Key(LuaTableAddr, LuaFuncAddr);
        CLuaAddr2DelegateMap::iterator itDelegate = m_LuaAddr2DelegateMap.find(Key);
        if (itDelegate != m_LuaAddr2DelegateMap.end())
        {
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
        Delegate->m_Key = Key;

        int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
        Delegate->FunctionRef = CallbackRef;
        Delegate->ParamCount = 1;
        Delegate->CallbackParams[0] = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
        Delegate->ParamsValudAddr[0] = LuaTableAddr;

        Delegate->Object = Object;
        Delegate->Outer = Outer;
        MakeScriptDelegate(Delegate, LuaFuncAddr, Outer, DynamicProperty);

        Delegate->DynamicProperty = GetDynamicProperty(DynamicProperty->SafePropertyPtr->GetProperty());
        Delegate->DynamicFunc = GetDynamicFunction(Delegate->Function);

        AddBindLuaFunction(Delegate, Object);
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
    FCLuaDelegateKey Key(Object, LuaFuncAddr);
    CLuaAddr2DelegateMap::iterator itDelegate = m_LuaAddr2DelegateMap.find(Key);
    if (itDelegate != m_LuaAddr2DelegateMap.end())
    {
        lua_pop(L, 2);
        return Outer;
    }
    lua_pushvalue(L, -1);
    int CallbackRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表
    lua_pop(L, 2);

    FCLuaDelegate* Delegate = new FCLuaDelegate();
    Delegate->FunctionAddr = LuaFuncAddr;
    Delegate->m_Key = Key;
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

    AddBindLuaFunction(Delegate, Object);
    return Outer;
}

void   FCDynamicDelegateManager::DeleteAllDelegateByUObject(const UObjectBase* Object)
{
    CUObject2DelegateListMap::iterator itList = m_UObject2DelegateListMap.find(Object);
    if(itList != m_UObject2DelegateListMap.end())
    {
        FCLuaDelegateList DelegateList;
        DelegateList.swap(itList->second);
        m_UObject2DelegateListMap.erase(Object);

        DeleteDelegateList(DelegateList);
    }
}

void   FCDynamicDelegateManager::Clear()
{
    while(m_UObject2DelegateListMap.size() > 0)
    {
        CUObject2DelegateListMap::iterator itBegin = m_UObject2DelegateListMap.begin();
        FCLuaDelegateList DelegateList;
        DelegateList.swap(itBegin->second);
        m_UObject2DelegateListMap.erase(itBegin);

        DeleteDelegateList(DelegateList);
    }
    FC_ASSERT(m_LuaAddr2DelegateMap.size() != 0);
    FC_ASSERT(m_UFunction2DelegateListMap.size() != 0);
    FC_ASSERT(m_UObject2DelegateListMap.size() != 0);
    m_LuaAddr2DelegateMap.clear();
    m_UFunction2DelegateListMap.clear();
    m_UObject2DelegateListMap.clear();

    ReleasePtrMap(m_DynamicFuncMap);
    ReleasePtrMap(m_DynamicProperyMap);
}

FCDynamicOverrideFunction* FCDynamicDelegateManager::GetDynamicFunction(UFunction* Function)
{
    CAdr2DynamicFuncMap::iterator itFunc = m_DynamicFuncMap.find(Function);
    if(itFunc != m_DynamicFuncMap.end())
    {
        if(itFunc->second->m_bNeedReInit)
        {
            itFunc->second->InitParam(Function);
        }
        return itFunc->second;
    }
    FCDynamicOverrideFunction *DynamicFunc = new FCDynamicOverrideFunction();
    DynamicFunc->InitParam(Function);
    m_DynamicFuncMap[Function] = DynamicFunc;
    return DynamicFunc;
}

void  FCDynamicDelegateManager::AddBindLuaFunction(FCLuaDelegate* Delegate, UObject* Object)
{
    m_LuaAddr2DelegateMap[Delegate->m_Key] = Delegate;

    FCLuaDelegateListNode  *ListNode = NewDelegateListNode();
    ListNode->m_Delegate = Delegate;
    m_UObject2DelegateListMap[Object].push_back(ListNode);

    ListNode = NewDelegateListNode();
    ListNode->m_Delegate = Delegate;
    m_UFunction2DelegateListMap[Delegate->Function].push_back(ListNode);
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

void  FCDynamicDelegateManager::DeleteDelegateList(FCLuaDelegateList& DelegateList)
{
    while(DelegateList.size() > 0)
    {
        FCLuaDelegateListNode *BeginPtr = DelegateList.front_ptr();
        DelegateList.pop_front();

        m_LuaAddr2DelegateMap.erase(BeginPtr->m_Delegate->m_Key);

        CUFunction2DelegateListMap::iterator itList = m_UFunction2DelegateListMap.find(BeginPtr->m_Delegate->Function);
        if(itList != m_UFunction2DelegateListMap.end())
        {
            FCLuaDelegateList &FunctionDelegateList = itList->second;
            for(FCLuaDelegateList::iterator it = FunctionDelegateList.begin(); it != FunctionDelegateList.end(); ++it)
            {
                if(it->m_Delegate == BeginPtr->m_Delegate)
                {
                    FCLuaDelegateListNode *itNode = it.get_ptr();
                    FunctionDelegateList.erase(it);
                    DelDelgateListNode(itNode);
                    break;
                }
            }
            if(FunctionDelegateList.empty())
            {
                m_UFunction2DelegateListMap.erase(itList);
            }
        }
        DeleteLuaDelegate(BeginPtr->m_Delegate);
        DelDelgateListNode(BeginPtr);
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

    CUFunction2DelegateListMap::iterator itDeletateList = m_UFunction2DelegateListMap.find(Delegate->Function);
    int  DeletateCount = 0;
    if(itDeletateList != m_UFunction2DelegateListMap.end())
    {
        DeletateCount = itDeletateList->second.size();
    }
    if(DeletateCount == 0)
    {
        UFunction* FindFunc = Delegate->OuterClass->FindFunctionByName(Delegate->DynamicFunc->Name);
        if (FindFunc && FindFunc == Delegate->Function)
        {
            //Delegate->OuterClass->RemoveFunctionFromFunctionMap(Delegate->Function);
            GetScriptContext()->RemoveOverideFunction(Delegate->OuterClass, Delegate->Function);
        }
        else
        {
            check(false);
            UE_LOG(LogTemp, Warning, TEXT("[FCDynamicDelegateManager]DeleteLuaDelegate, invalid function[%s], DeletateCount=%d"), UTF8_TO_TCHAR(Delegate->DynamicFunc->Name), DeletateCount);
        }
    }

    ObjRefKey Key(nullptr, Delegate);
    FCObjRef *ObjRef = FCGetObj::GetIns()->FindObjRefByKey(Key);
    if(ObjRef)
    {
        ObjRef->Ref = 1;
        FCGetObj::GetIns()->DeleteValue(ObjRef->PtrIndex);
    }
    // 这个地方也许要做一个标记，如果还在Lua中引用，就不能立即释放
    //if(Delegate->DynamicProperty)
    //{
    //    delete Delegate->DynamicProperty;
    //}
    //if (Delegate->DynamicFunc)
    //{
    //    delete Delegate->DynamicFunc;
    //}
    if(Delegate->DynamicFunc)
    {
        Delegate->DynamicFunc->m_bNeedReInit = true;
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
    UFunction* LuaFunction = MakeReplaceFunction(DynamicProperty->SafePropertyPtr->GetSignatureFunction(), OuterClass, FuncName, FCDynamicDelegate_CallLua);

    Delegate->Delegate.BindUFunction(Outer, FuncName);
    Delegate->Function = LuaFunction;
    Delegate->OuterClass = OuterClass;
};

UFunction* FCDynamicDelegateManager::MakeReplaceFunction(UFunction* SrcFunction, UClass* OuterClass, const FName& NewFuncName, FNativeFuncPtr InFunc)
{
    UFunction *DumpFunc = FindOrDumpFunction(SrcFunction, OuterClass, NewFuncName);
    DumpFunc->SetNativeFunc(InFunc);
    return DumpFunc;
}

FCLuaDelegateListNode* FCDynamicDelegateManager::NewDelegateListNode()
{
    return new FCLuaDelegateListNode();
}

void FCDynamicDelegateManager::DelDelgateListNode(FCLuaDelegateListNode* pNode)
{
    if(pNode)
    {
        delete pNode;
    }
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
        FCLuaDelegateList* DelegateList = FCDynamicDelegateManager::GetIns().FindDelegateByFunction(Func);
        bool bInitParam = false;
        bool bZeroParam = false;

        FCLuaDelegateListNode *DelegateListNode = DelegateList ? DelegateList->front_ptr() : nullptr;

        while(DelegateListNode)
        {
            FCLuaDelegate *Delegate = DelegateListNode->m_Delegate;
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
                    FCCallScriptDelegate(ScriptContext->m_LuaState, Delegate->Object, Delegate->ScriptIns, *Delegate, Delegate->DynamicFunc, TheStack);
                }
            }
            else
            {
                FCCallScriptDelegate(ScriptContext->m_LuaState, Delegate->Object, Delegate->ScriptIns, *Delegate, Delegate->DynamicFunc, TheStack);
            }

            DelegateListNode = DelegateListNode->m_pNext;
        }
    }
}