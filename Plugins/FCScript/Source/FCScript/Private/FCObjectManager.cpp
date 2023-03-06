#include "FCObjectManager.h"
#include "FCDynamicOverrideFunc.h"
#include "FCCallScriptFunc.h"
#include "FCRunTimeRegister.h"
#include "FCObjectUseFlag.h"

extern uint8 GRegisterNative(int32 NativeBytecodeIndex, const FNativeFuncPtr& Func);

FFCObjectdManager::FFCObjectdManager():m_pCurrentBindClass(nullptr)
{
    // 先注册一下全局Native函数
    GRegisterNative(EX_CallFCBeginPlay, FCDynamicOverrideBeginBeginPlay);
    GRegisterNative(EX_CallFCOverride, FCDynamicOverrideNative);
    GRegisterNative(EX_CallFCDelegate, FCDynamicOverrideDelegate);
}

FFCObjectdManager::~FFCObjectdManager()
{

}

static FFCObjectdManager  ObjectMgrIns;
FFCObjectdManager  *FFCObjectdManager::GetSingleIns()
{
	return &ObjectMgrIns;
}

void  FFCObjectdManager::Clear()
{
	m_DynamicBindClassInfo.clear();
	m_pCurrentBindClass = nullptr;
	m_ScriptsClassName = nullptr;
	m_BindObjects.clear();
    m_BindScriptInsMap.clear();
    m_DelegateRefMap.clear();

    m_OverrideFunctionScriptInsMap.clear();
    m_OverrideObjectFunctionMap.clear();
    m_OverrideRefMap.clear();
    m_DelayCallBeginPlayList.clear();

	//-------------------------------	
	ClearAllDynamicFunction();
}

void  FFCObjectdManager::BindScript(const class UObjectBaseUtility *Object, UClass *Class, const FString &ScriptClassName)
{
	const char *Name = TCHAR_TO_UTF8(*ScriptClassName);
	return BindToScript(Object, Class, Name);
}

void  FFCObjectdManager::BindToScript(const class UObjectBaseUtility* Object, UClass* Class, const char* ScriptClassName)
{
	ScriptClassName = GetConstName(ScriptClassName);
    FCObjectUseFlag::GetIns().Ref(Object);
	FBindObjectInfo &Info = m_BindObjects[Object];
	Info.Set(Object, Object->GetLinkerIndex(), ScriptClassName, LUA_NOREF);
	RegisterReceiveBeginPlayFunction((UObject*)Object, Class);
}

void  FFCObjectdManager::CallBindScript(UObject *InObject, const char *ScriptClassName, int InitializerTableRef)
{
    ScriptClassName = GetConstName(ScriptClassName);
    FCObjectUseFlag::GetIns().Ref(InObject);
    FBindObjectInfo &Info = m_BindObjects[InObject];
    Info.Set(InObject, InObject->GetLinkerIndex(), ScriptClassName, InitializerTableRef);
    Info.m_ScriptIns = FCDynamicBindScript(InObject);
    FCScriptContext  *ScriptContext = GetScriptContext();
    CallAnyScriptFunc(ScriptContext, Info.m_ScriptIns, "UserConstructionScript");
    CallAnyScriptFunc(ScriptContext, Info.m_ScriptIns, "ReceiveBeginPlay");
}

void  FFCObjectdManager::DynamicBind(const class UObjectBaseUtility *Object, UClass *Class)
{
	if(Class == m_pCurrentBindClass && m_ScriptsClassName)
	{
		BindToScript(Object, Class, m_ScriptsClassName);
	}
}

void  FFCObjectdManager::NotifyDeleteUObject(const class UObjectBase* Object, int32 Index)
{
	CBindObjectInfoMap::iterator itBind = m_BindObjects.find(Object);
	if(itBind != m_BindObjects.end())
	{
		FCScriptContext  *ScriptContext = GetScriptContext();
		FBindObjectInfo  &BindInfo = itBind->second;
		lua_State* L = ScriptContext->m_LuaState;
        int64 ObjID = BindInfo.m_ObjRefID;

		CallAnyScriptFunc(GetScriptContext(), BindInfo.m_ScriptIns, "ReceiveBeginDestroy");

		luaL_unref(L, LUA_REGISTRYINDEX, BindInfo.m_ScriptIns);
        m_BindScriptInsMap.erase(m_BindScriptInsMap.find(ObjID));
		BindInfo.m_ScriptIns = 0;
		m_BindObjects.erase(itBind);

        // 释放
        RemoveOverrideRefByObject(Object);
	}
	ClearObjectDelegate(Object);
}

void  FFCObjectdManager::PushDynamicBindClass(UClass* Class, const char *ScriptClassName)
{
	ScriptClassName = GetConstName(ScriptClassName);
	FDynmicBindClassInfo  Info = {Class, ScriptClassName};
	m_DynamicBindClassInfo.push_back(Info);
	m_pCurrentBindClass = Class;
	m_ScriptsClassName = ScriptClassName;
}

void  FFCObjectdManager::PopDynamicBindClass()
{
	m_DynamicBindClassInfo.pop_back();
	if(m_DynamicBindClassInfo.size() > 0)
	{
		const FDynmicBindClassInfo  &Info = m_DynamicBindClassInfo.back();
		m_pCurrentBindClass = Info.Class;
		m_ScriptsClassName = Info.ScriptClassName;
	}
	else
	{
		m_pCurrentBindClass = nullptr;
		m_ScriptsClassName = nullptr;
	}
}

bool  FFCObjectdManager::IsDynamicBindClass(UClass *Class)
{
	return m_pCurrentBindClass == Class;
}

//----------------------------------------------------------------------

FCDynamicOverrideFunction *FFCObjectdManager::RegisterReceiveBeginPlayFunction(UObject *InObject, UClass* Class)
{
	FCDynamicClassDesc *ClassDesc = GetScriptContext()->RegisterUStruct(InObject->GetClass());
	if(!ClassDesc)
	{
		return nullptr;
	}
	FCDynamicFunction *ClassFunc = ClassDesc->FindFunctionByName("ReceiveBeginPlay");
	if(ClassFunc)
	{
		FNativeFuncPtr OleNativeFuncPtr = ClassFunc->Function->GetNativeFunc();
        FCDynamicBindScript(InObject);
		FCDynamicOverrideFunction *DynamicFunc = ToOverrideFunction(InObject, ClassFunc->Function, FCDynamicOverrideBeginBeginPlay, EX_CallFCBeginPlay);
		return DynamicFunc;
	}
	else
	{
		// 该类没有ReceiveBeginPlay，直接调用脚本中的, 一个坑，立即创建UObject, 指针是无效的，尝试调用里面的接口会崩溃，需要延迟一帧		
		int64 ScriptIns = FCDynamicBindScript(InObject);
		m_DelayCallBeginPlayList.push_back(InObject);
	}
	return nullptr;
}

FCDynamicOverrideFunction * FFCObjectdManager::RegisterOverrideFunc(UObject *InObject, int64 InScriptID, const char *InFuncName)
{
	FCDynamicClassDesc *ClassDesc = GetScriptContext()->RegisterUStruct(InObject->GetClass());
	if(!ClassDesc)
	{
		return nullptr;
	}
	FCDynamicFunction *ClassFunc = ClassDesc->FindFunctionByName(InFuncName);
	if(ClassFunc)
	{
        UFunction  *Function = ClassFunc->Function;
		FCDynamicOverrideFunction * DynamicFunc = ToOverrideFunction(InObject, Function, FCDynamicOverrideNative, EX_CallFCOverride);

        FScriptOverrideKey  Key(InObject, Function);
        COverrideFunction2ScriptInsMap::iterator itOverride = m_OverrideFunctionScriptInsMap.find(Key);
        if(itOverride == m_OverrideFunctionScriptInsMap.end())
        {
            m_OverrideFunctionScriptInsMap[Key] = InScriptID;
            CScriptFunctionList &FuncList = m_OverrideObjectFunctionMap[InObject];
            FuncList.push_back(Function);
            ++(m_OverrideRefMap[Function]);
        }
		return DynamicFunc;
	}
	return nullptr;
}

FCDynamicOverrideFunction * FFCObjectdManager::ToOverrideFunction(UObject *InObject, UFunction *InFunction, FNativeFuncPtr InFuncPtr, int InNativeBytecodeIndex)
{
	COverrideFunctionMap::iterator itFunc = m_OverrideFunctionMap.find(InFunction);
    FCDynamicOverrideFunction* DynamicFunc = nullptr;
	if(itFunc != m_OverrideFunctionMap.end())
	{
        DynamicFunc = itFunc->second;
	}
    else
    {
        DynamicFunc = new FCDynamicOverrideFunction();
        DynamicFunc->InitParam(InFunction);
        DynamicFunc->OleNativeFuncPtr = InFunction->GetNativeFunc();
        DynamicFunc->CurOverrideFuncPtr = InFuncPtr;
        DynamicFunc->m_NativeBytecodeIndex = InNativeBytecodeIndex;
        DynamicFunc->m_NativeScript = InFunction->Script;
        DynamicFunc->LuaFunctionMame = DynamicFunc->Name;
        //DynamicFunc->m_OverideName = FName(DynamicFunc->Name);
        DynamicFunc->m_OverideName = DynamicFunc->Function->GetFName();

        m_OverrideFunctionMap[InFunction] = DynamicFunc;
    }

    if(!DynamicFunc->m_bNeedRestoreNative)
    {
        DynamicFunc->m_bNeedRestoreNative = true;

        TArray<uint8> Script;
        Script.Add(InNativeBytecodeIndex);
        Script.Add(EX_Return);
        Script.Add(EX_Nothing);
        Script.Add(EX_Return);
        Script.Add(EX_Nothing);
        Script += DynamicFunc->m_NativeScript;

        InFunction->Script.Empty();
        InFunction->Script = Script;

        InFunction->SetNativeFunc(InFuncPtr);
    }

	return DynamicFunc;
}

FCDynamicOverrideFunction * FFCObjectdManager::FindOverrideFunction(UObject *InObject, UFunction *InFunction)
{
	COverrideFunctionMap::iterator itFunc = m_OverrideFunctionMap.find(InFunction);
	if(itFunc != m_OverrideFunctionMap.end())
	{
		return itFunc->second;
	}
	return NULL;
}

int64 FFCObjectdManager::FindOverrideScriptIns(UObject *InObject, UFunction *InFunction)
{
    FScriptOverrideKey  Key(InObject, InFunction);
    COverrideFunction2ScriptInsMap::iterator itOverride = m_OverrideFunctionScriptInsMap.find(Key);
    if(itOverride != m_OverrideFunctionScriptInsMap.end())
    {
        return itOverride->second;
    }
    return 0;
}

int  FFCObjectdManager::NativeCall(UObject* InObject, FCDynamicFunction* DynamicFunc, lua_State* L, int nStart)
{
	uint8   Buffer[256];
	int     RetCount = 0;
	COverrideFunctionMap::iterator itFunc = m_OverrideFunctionMap.find(DynamicFunc->Function);
	if (itFunc != m_OverrideFunctionMap.end())
	{
		FCDynamicOverrideFunction* OverideFunc = itFunc->second;
		if (OverideFunc->m_bLockCall)
		{
			return 0;
		}
        // 如果没有原生的函数，空的蓝图的接口
        if (OverideFunc->m_NativeScript.IsEmpty() && OverideFunc->Function->Script.Num() == 5)
        {
            return 0;
        }
		OverideFunc->m_bLockCall = true;
		FNativeFuncPtr OldFuncPtr = OverideFunc->Function->GetNativeFunc();
		TArray<uint8>& Script = OverideFunc->Function->Script;
		Script[0] = EX_Nothing;        
        Script[1] = EX_Nothing;
		Script[3] = EX_Nothing;
		RetCount = WrapNativeCallFunction(L, nStart, InObject, OverideFunc, Buffer, sizeof(Buffer), OverideFunc->OleNativeFuncPtr);
		Script[0] = OverideFunc->m_NativeBytecodeIndex;
		Script[1] = EX_Return;
		Script[3] = EX_Return;

		OverideFunc->m_bLockCall = false;
	}
	else
	{
		FNativeFuncPtr OldFuncPtr = DynamicFunc->Function->GetNativeFunc();
		RetCount = WrapNativeCallFunction(L, nStart, InObject, DynamicFunc, Buffer, sizeof(Buffer), OldFuncPtr);
	}
	return RetCount;
}

FCDynamicDelegateList  *FFCObjectdManager::FindDelegateFunction(UObject *InObject)
{
	CObjectDelegateMap::iterator itFunc = m_ObjectDelegateMap.find(InObject);
	if(itFunc != m_ObjectDelegateMap.end())
	{
		return &(itFunc->second);
	}
	return nullptr;
}

void  FFCObjectdManager::RegisterScriptDelegate(UObject *InObject, const FCDynamicProperty* InDynamicProperty, const void* InFuncAddr, int InFunctionRef, const int* InParams, int InParamCount)
{
	if(!InObject || !InDynamicProperty)
	{
		return ;
	}

	UFunction* Func = InDynamicProperty->GetSignatureFunction();
	if(!Func)
	{
		return ;
	}
    FName  FuncName(InDynamicProperty->GetFieldName());
    Func = FindOrDumpFunction(Func, InObject->GetClass(), FuncName, FCDynamicOverrideDelegate);

	FCDynamicOverrideFunction *DynamicFunc = this->ToOverrideFunction(InObject, Func, FCDynamicOverrideDelegate, EX_CallFCDelegate);

	FCDynamicDelegateList  &DelegateList = m_ObjectDelegateMap[InObject];
    FCObjectUseFlag::GetIns().Ref(InObject);
	FCDelegateInfo  Info(DynamicFunc, InDynamicProperty, InFuncAddr, InFunctionRef, InParams, InParamCount);
	if(!DelegateList.AddScriptDelegate(Info))
	{
		return ;
	}
	int Ref = m_DelegateRefMap[Func];
	m_DelegateRefMap[Func] = Ref + 1;
	if(0 == Ref)
	{
		// 添加引用吧
		AddDelegateToClass(DynamicFunc, InObject->GetClass());
	}
	
    FuncName = DynamicFunc->m_OverideName;
	uint8* ObjAddr = (uint8 *)InObject;
	uint8* ValueAddr = ObjAddr + InDynamicProperty->Offset_Internal;
	if(InDynamicProperty->Type == FCPropertyType::FCPROPERTY_MulticastDelegateProperty)
	{
		FMulticastDelegateProperty* DelegateProperty = (FMulticastDelegateProperty*)InDynamicProperty->Property;		
		FScriptDelegate DynamicDelegate;
		DynamicDelegate.BindUFunction(InObject, FuncName);

		FMulticastScriptDelegate& MulticastDelegate = (*(FMulticastScriptDelegate*)ValueAddr);
		MulticastDelegate.AddUnique(MoveTemp(DynamicDelegate));
	}
	else if(FCPROPERTY_DelegateProperty == InDynamicProperty->Type)
	{
		FDelegateProperty* DelegateProperty = (FDelegateProperty*)InDynamicProperty->Property;
		FScriptDelegate& ScriptDelegate = (*(FScriptDelegate*)ValueAddr);
		ScriptDelegate.BindUFunction(InObject, FuncName);
	}
    else if(FCPROPERTY_MulticastSparseDelegateProperty == InDynamicProperty->Type)
    {
        FMulticastSparseDelegateProperty * DelegateProperty = (FMulticastSparseDelegateProperty*)InDynamicProperty->Property;
        FSparseDelegate & ScriptDelegate = (*(FSparseDelegate*)ValueAddr);

        FScriptDelegate DynamicDelegate;
        DynamicDelegate.BindUFunction(InObject, FuncName);
        ScriptDelegate.__Internal_AddUnique(InObject, FuncName, DynamicDelegate);
    }
}

void  FFCObjectdManager::RemoveScriptDelegate(UObject *InObject, const FCDynamicProperty* InDynamicProperty, const void* InFuncAddr)
{
	if(!InObject || !InDynamicProperty)
	{
		return ;
	}
	CObjectDelegateMap::iterator itDelegateList = m_ObjectDelegateMap.find(InObject);
	if(itDelegateList == m_ObjectDelegateMap.end())
	{
		return ;
	}	
	UFunction* Func = InDynamicProperty->GetSignatureFunction();
	if(!Func)
	{
		return ;
	}

	FCDynamicOverrideFunction *DynamicFunc = this->FindOverrideFunction(InObject, Func);
	if(!DynamicFunc)
	{
		return ;
	}
	FCDynamicDelegateList  &DelegateList = itDelegateList->second;
	FCDelegateInfo  Info(DynamicFunc, InDynamicProperty, InFuncAddr, 0, nullptr, 0);
	if(!DelegateList.DelScriptDelegate(Info))
	{
		return ;
	}
	int Ref = m_DelegateRefMap[Func];
	m_DelegateRefMap[Func] = Ref - 1;
	if(0 == Ref)
	{
		// 释放引用吧
		RemoveDelegateFromClass(DynamicFunc, InObject->GetClass());
	}

	// 统计一下数量
	int nDelegateCount = 0;
	for(int i = 0; i<DelegateList.Delegates.size(); ++i)
	{
		if(DelegateList.Delegates[i].DynamicFunc == DynamicFunc)
		{
			++nDelegateCount;
		}
	}
	
	if(nDelegateCount == 0)
	{
		RemoveObjectDelegate(InObject, InDynamicProperty, DynamicFunc);
	}
}

void  FFCObjectdManager::ClearScriptDelegate(UObject* InObject, const FCDynamicProperty* InDynamicProperty)
{
	if (!InObject || !InDynamicProperty)
	{
		return;
	}
	CObjectDelegateMap::iterator itDelegateList = m_ObjectDelegateMap.find(InObject);
	if (itDelegateList == m_ObjectDelegateMap.end())
	{
		return;
	}
	UFunction* Func = InDynamicProperty->GetSignatureFunction();
	if (!Func)
	{
		return;
	}
	FCDynamicOverrideFunction * DynamicFunc = this->FindOverrideFunction(InObject, Func);
	if (!DynamicFunc)
	{
		return;
	}
	FCDynamicDelegateList& DelegateList = itDelegateList->second;
	for (int i = DelegateList.Delegates.size() - 1; i >= 0; --i)
	{
		const FCDelegateInfo& DelegateInfo = DelegateList.Delegates[i];
		if (DelegateInfo.DynamicFunc == DynamicFunc)
		{
			Func = DelegateInfo.DynamicFunc->Function;
			int Ref = m_DelegateRefMap[Func];
			m_DelegateRefMap[Func] = Ref - 1;
			if (0 == Ref)
			{
				// 释放引用吧
				RemoveDelegateFromClass(DynamicFunc, InObject->GetClass());
			}
			DelegateList.Delegates.erase(DelegateList.Delegates.begin() + i);
		}
	}
	RemoveDelegateFromClass(DynamicFunc, InObject->GetClass());
    RemoveObjectDelegate(InObject, InDynamicProperty, DynamicFunc);
}

void  FFCObjectdManager::ClearObjectDelegate(const class UObjectBase *Object)
{
	CObjectDelegateMap::iterator itDelegateList = m_ObjectDelegateMap.find((UObject*)Object);
	if(itDelegateList != m_ObjectDelegateMap.end())
	{
		lua_State *L = GetScriptContext()->m_LuaState;

		UObject *InObject = (UObject *)Object;
		UClass  *Class = InObject->GetClass();
		FCDynamicDelegateList  &DelegateList = itDelegateList->second;
		for(int i = DelegateList.Delegates.size() - 1; i>=0; --i)
		{
			FCDelegateInfo &Info = DelegateList.Delegates[i];
			UFunction* Func = Info.DynamicFunc->Function;
			int Ref = m_DelegateRefMap[Func] - 1;
			m_DelegateRefMap[Func] = Ref;			
			RemoveObjectDelegate(InObject, Info.DynamicProperty, Info.DynamicFunc);

			// 释放lua的引入变量
			for (int iParam = 0; iParam < Info.ParamCount; ++iParam)
			{
				luaL_unref(L, LUA_REGISTRYINDEX, Info.CallbackParams[iParam]);
			}
			luaL_unref(L, LUA_REGISTRYINDEX, Info.FunctionRef);
			Info.ParamCount = 0;
			Info.FunctionRef = -1;

			if(Ref <= 0)
			{
				// 没有地方引用了, 需要还原NativeFuncPtr
                if(Info.DynamicFunc->m_bNeedRestoreNative)
                {
                    Info.DynamicFunc->m_bNeedRestoreNative = false;
                    Func->Script = Info.DynamicFunc->m_NativeScript;
                    Func->SetNativeFunc(Info.DynamicFunc->OleNativeFuncPtr);

                    RemoveDelegateFromClass(Info.DynamicFunc, Class);
                }

                // 不要立即释放，因为可能还有地方引用这个，在Clear时延迟释放吧

				//m_OverrideFunctionMap.erase(Func);
				//delete Info.DynamicFunc;
			}
		}
		m_ObjectDelegateMap.erase(itDelegateList);
	}
}

void  FFCObjectdManager::AddDelegateToClass(FCDynamicOverrideFunction *InDynamicFunc, UClass *InClass)
{
	UFunction  *Function = InDynamicFunc->Function;
	if(InDynamicFunc->OleNativeFuncPtr != FCDynamicOverrideDelegate)
	{
		Function->SetNativeFunc(FCDynamicOverrideDelegate);
	}
    if(Function->Script.Num() < 1)
    {
        Function->Script.Add(EX_CallFCDelegate);
        Function->Script.Add(EX_Return);
        Function->Script.Add(EX_Nothing);
    }			
    FC_ASSERT(InDynamicFunc->m_BindClass != nullptr);
    InDynamicFunc->m_BindClass = InClass;
    //FC_ASSERT(InClass->FindFunctionByName(Function->GetFName()) != nullptr);

	//InClass->AddFunctionToFunctionMap(Function, Function->GetFName());
}

void  FFCObjectdManager::RemoveDelegateFromClass(FCDynamicOverrideFunction *InDynamicFunc, UClass *InClass)
{
	UFunction  *Function = InDynamicFunc->Function;
	Function->SetNativeFunc(InDynamicFunc->OleNativeFuncPtr);
	Function->Script = InDynamicFunc->m_NativeScript;
	InClass->RemoveFunctionFromFunctionMap(Function);
}

void  FFCObjectdManager::RemoveObjectDelegate(UObject *InObject, const FCDynamicProperty* InDynamicProperty, const FCDynamicOverrideFunction* InDynamicFunc)
{	
	uint8* ObjAddr = (uint8 *)InObject;
	uint8* ValueAddr = ObjAddr + InDynamicProperty->Offset_Internal;
	if(FCPropertyType::FCPROPERTY_MulticastDelegateProperty == InDynamicProperty->Type)
	{		
		FMulticastScriptDelegate& MulticastDelegate = (*(FMulticastScriptDelegate*)ValueAddr);
		MulticastDelegate.Clear();
	}
	else if(FCPROPERTY_DelegateProperty == InDynamicProperty->Type)
	{
		FScriptDelegate& ScriptDelegate = (*(FScriptDelegate*)ValueAddr);
		ScriptDelegate.Clear();
	}
    else if(FCPROPERTY_MulticastSparseDelegateProperty == InDynamicProperty->Type)
    {
        FMulticastSparseDelegateProperty* DelegateProperty = (FMulticastSparseDelegateProperty*)InDynamicProperty->Property;
        FSparseDelegate& ScriptDelegate = (*(FSparseDelegate*)ValueAddr);
        ScriptDelegate.__Internal_Clear(InObject, InDynamicFunc->m_OverideName);
    }
}

void  FFCObjectdManager::RemoveOverrideRefByObject(const class UObjectBase *Object)
{
    COverrideObjectFunctionMap::iterator itFuncList = m_OverrideObjectFunctionMap.find((UObjectBase*)Object);
    if (itFuncList != m_OverrideObjectFunctionMap.end())
    {
        const CScriptFunctionList &FuncList = itFuncList->second;
        FScriptOverrideKey Key;
        Key.Object = Object;
        for (int i = 0; i < FuncList.size(); ++i)
        {
            Key.Function = FuncList[i];
            m_OverrideFunctionScriptInsMap.erase(Key);
            CFunctionRefMap::iterator itRef = m_OverrideRefMap.find(Key.Function);
            if (itRef != m_OverrideRefMap.end())
            {
                --(itRef->second);
                if (itRef->second <= 0)
                {
                    m_OverrideRefMap.erase(itRef);
                }
            }
        }
        m_OverrideObjectFunctionMap.erase(itFuncList);
    }
}

void  FFCObjectdManager::ClearAllDynamicFunction()
{
	// 还原函数指针
	for (COverrideFunctionMap::iterator itOverride = m_OverrideFunctionMap.begin(); itOverride != m_OverrideFunctionMap.end(); ++itOverride)
	{
		FCDynamicOverrideFunction* DynamicFunc = itOverride->second;
        if(DynamicFunc->m_bNeedRestoreNative)
        {
            DynamicFunc->m_bNeedRestoreNative = false;
            UFunction* NativeFunction = DynamicFunc->Function;
            NativeFunction->Script = DynamicFunc->m_NativeScript;
            NativeFunction->SetNativeFunc(DynamicFunc->OleNativeFuncPtr);

            if(DynamicFunc->m_BindClass)
            {   
                RemoveDelegateFromClass(DynamicFunc, DynamicFunc->m_BindClass);
            }
        }
	}
	ReleasePtrMap(m_OverrideFunctionMap);

	m_ObjectDelegateMap.clear();
	m_DelegateRefMap.clear();
}

void  FFCObjectdManager::CheckGC()
{
	// Debug 测试
	int InvalidCount = 0;
	for(COverrideFunctionMap::iterator itFunc = m_OverrideFunctionMap.begin(); itFunc != m_OverrideFunctionMap.end(); ++itFunc)
	{
		if(!IsValid(itFunc->first))
		{
			++InvalidCount;
		}
	}
	for(size_t Size = m_DelayCallBeginPlayList.size(), i = 0; i<Size; ++i)
	{
		UObject* InObject = m_DelayCallBeginPlayList[i];
		int64 ScriptIns = FCDynamicBindScript(InObject);
		CallAnyScriptFunc(GetScriptContext(), ScriptIns, "ReceiveBeginPlay");
	}
	m_DelayCallBeginPlayList.clear();
}
//----------------------------------------------------------------------
