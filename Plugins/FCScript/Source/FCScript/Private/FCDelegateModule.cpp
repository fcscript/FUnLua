#include "FCDelegateModule.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Templates/Casts.h"
#include "Logging/LogMacros.h"
#include "Misc/EmbeddedCommunication.h"

#include "FCScriptInterface.h"
#include "FCDynamicClassDesc.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCRefObjCache.h"
#include "FCLoadScript.h"
#include "FCCallScriptFunc.h"
#include "FCTemplateType.h"
#include "FCScriptDelegates.h"
#include "FCDefaultParam.h"
#include "FCDynamicDelegateManager.h"

#include "Interfaces/IPluginManager.h"

#if WITH_EDITOR
#include "Editor.h"
#include "GameDelegates.h"
#endif

void FFCDelegateModule::OnStartupModule()
{
	FWorldDelegates::OnWorldCleanup.AddRaw(this, &FFCDelegateModule::OnWorldCleanup);
	FWorldDelegates::OnPostWorldCleanup.AddRaw(this, &FFCDelegateModule::OnPostWorldCleanup);
	FWorldDelegates::OnPreWorldInitialization.AddRaw(this, &FFCDelegateModule::OnPreWorldInitialization);
	FWorldDelegates::OnPostWorldInitialization.AddRaw(this, &FFCDelegateModule::OnPostWorldInitialization);
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FFCDelegateModule::OnPostEngineInit);   // called before FCoreDelegates::OnFEngineLoopInitComplete.Broadcast(), after GEngine->Init(...)
	FCoreDelegates::OnPreExit.AddRaw(this, &FFCDelegateModule::OnPreExit);                 // called before StaticExit()
	FCoreDelegates::OnAsyncLoadingFlushUpdate.AddRaw(this, &FFCDelegateModule::OnAsyncLoadingFlushUpdate);
	FCoreDelegates::OnHandleSystemError.AddRaw(this, &FFCDelegateModule::OnCrash);
	FCoreDelegates::OnHandleSystemEnsure.AddRaw(this, &FFCDelegateModule::OnCrash);
	FCoreUObjectDelegates::PreLoadMap.AddRaw(this, &FFCDelegateModule::PreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &FFCDelegateModule::PostLoadMapWithWorld);

#if WITH_EDITOR
	// delegates for PIE
	FEditorDelegates::PreBeginPIE.AddRaw(this, &FFCDelegateModule::PreBeginPIE);
	FEditorDelegates::BeginPIE.AddRaw(this, &FFCDelegateModule::BeginPIE);
	FEditorDelegates::PostPIEStarted.AddRaw(this, &FFCDelegateModule::PostPIEStarted);
	FEditorDelegates::PrePIEEnded.AddRaw(this, &FFCDelegateModule::PrePIEEnded);
	FEditorDelegates::EndPIE.AddRaw(this, &FFCDelegateModule::EndPIE);
	FGameDelegates::Get().GetEndPlayMapDelegate().AddRaw(this, &FFCDelegateModule::OnEndPlayMap);
#endif
}

//-------------------------------------------------------------
void FFCDelegateModule::OnWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources)
{

}

void FFCDelegateModule::OnPostWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources)
{

}

void FFCDelegateModule::OnPreWorldInitialization(UWorld *World, const UWorld::InitializationValues)
{

}

void FFCDelegateModule::OnPostWorldInitialization(UWorld *World, const UWorld::InitializationValues)
{
	if(bStartInit && !Ticker)
	{
		if(GEngine->GameViewport)
		{
			GameInstance = GEngine->GameViewport->GetGameInstance();
			if(GameInstance)
			{
				Ticker = NewObject<UFCTicker>(GameInstance);
				Ticker->AddToRoot();
                GetScriptContext()->m_Ticker = Ticker;

                DelegateObject = NewObject<UFCDelegateObject>(GameInstance);
                DelegateObject->AddToRoot();
                GetScriptContext()->m_DelegateObject = DelegateObject;
			}
		}
	}
}

void FFCDelegateModule::OnPostEngineInit()
{
	if (!GIsEditor)
	{
		Startup();
	}
}

void FFCDelegateModule::OnPreExit()
{
	Shutdown();
}

void FFCDelegateModule::OnAsyncLoadingFlushUpdate()
{
    constexpr EInternalObjectFlags AsyncObjectFlags = EInternalObjectFlags::AsyncLoading | EInternalObjectFlags::Async;

    TArray<FWeakObjectPtr> CandidatesTemp;
    TArray<UObject*> CandidatesActive; // Only touch objects which are active and loaded
    TArray<int> CandidatesRemovedIndexes;

    {
        {
            FScopeLock Lock(&CandidatesCS);
            CandidatesTemp.Append(Candidates);
        }

        for (int32 i = CandidatesTemp.Num() - 1; i >= 0; --i)
        {
            FWeakObjectPtr ObjectPtr = CandidatesTemp[i];
            if (!ObjectPtr.IsValid())
            {
                // discard invalid objects
                CandidatesRemovedIndexes.Add(i);
                continue;
            }

            UObject* Object = ObjectPtr.Get();
            if (Object->HasAnyFlags(RF_NeedPostLoad)
                || Object->HasAnyInternalFlags(AsyncObjectFlags)
                || Object->GetClass()->HasAnyInternalFlags(AsyncObjectFlags))
            {
                // Delay bind on next update 
                continue;
            }

            CandidatesActive.Add(Object);
            CandidatesRemovedIndexes.Add(i);
        }
    }

    {
        FScopeLock Lock(&CandidatesCS);
        for (int32 j = 0; j < CandidatesRemovedIndexes.Num(); ++j)
        {
            Candidates.RemoveAt(CandidatesRemovedIndexes[j]);
        }
    }

    for (int32 i = CandidatesActive.Num() - 1; i >= 0; --i)
    {
        UObject* Object = CandidatesActive[i];
        if (Object->IsPendingKill())
        {
            continue;
        }
        UClass *ObjClass = Object->GetClass();
        UFunction * InterfaceFunc = GetScriptNameFunction(ObjClass);

        if(InterfaceFunc)
        {
            FString ScriptClassName;
            UObject* DefaultObject = ObjClass->GetDefaultObject();             // get CDO
            DefaultObject->UObject::ProcessEvent(InterfaceFunc, &ScriptClassName);   // force to invoke UObject::ProcessEvent(...)
            if (ScriptClassName.Len() > 0)
            {
                // 绑定一个UObject到脚本对象, 脚本的类名不可以为空串
                FFCObjectdManager::GetSingleIns()->BindScript(Object, ObjClass, ScriptClassName);
            }
        }
    }
}

void FFCDelegateModule::OnCrash()
{

}

void FFCDelegateModule::PreLoadMap(const FString &MapName)
{
	CallAnyScriptFunc(GetClientScriptContext(), 0, "OnPreLoadMap", MapName);
}

void FFCDelegateModule::PostLoadMapWithWorld(UWorld *World)
{
	CallAnyScriptFunc(GetClientScriptContext(), 0, "OnPostLoadMapWithWorld", World);
}

void FFCDelegateModule::OnPostGarbageCollect()
{

}

#if WITH_EDITOR
void FFCDelegateModule::PreBeginPIE(bool bIsSimulating)
{
	Startup();
}

void FFCDelegateModule::BeginPIE(bool bIsSimulating)
{

}

void FFCDelegateModule::PostPIEStarted(bool bIsSimulating)
{
	if(GEngine->GameViewport)
	{
		GameInstance = GEngine->GameViewport->GetGameInstance();
		if(GameInstance && !Ticker)
		{
			Ticker = NewObject<UFCTicker>(GameInstance);
			Ticker->AddToRoot();
            GetScriptContext()->m_Ticker = Ticker;

            DelegateObject = NewObject<UFCDelegateObject>(GameInstance);
            DelegateObject->AddToRoot();
            GetScriptContext()->m_DelegateObject = DelegateObject;
		}
	}
}

void FFCDelegateModule::PrePIEEnded(bool bIsSimulating)
{

}

void FFCDelegateModule::EndPIE(bool bIsSimulating)
{

}

void FFCDelegateModule::OnEndPlayMap()
{
	Shutdown();
}
#endif

//-------------------------------------------------------------

void FFCDelegateModule::NotifyUObjectCreated(const class UObjectBase *InObject, int32 Index)
{
	UObjectBaseUtility *Object = (UObjectBaseUtility*)InObject;
	TryBindScript(Object);

	// 尝试替换本地Player的控制器
	if (!Object->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) && Object->IsA<UInputComponent>())
	{
		AActor *Actor = Cast<APlayerController>(Object->GetOuter());
		if (!Actor)
		{
			Actor = Cast<APawn>(Object->GetOuter());
		}
		if (Actor && Actor->GetLocalRole() >= ROLE_AutonomousProxy)
		{
			CandidateInputComponents.AddUnique((UInputComponent*)InObject);
            FCObjectUseFlag::GetIns().Ref(InObject);
			if (!FWorldDelegates::OnWorldTickStart.IsBoundToObject(this))
			{
				OnWorldTickStartHandle = FWorldDelegates::OnWorldTickStart.AddRaw(this, &FFCDelegateModule::OnWorldTickStart);
			}
		}
	}
}

void FFCDelegateModule::OnUObjectArrayShutdown()
{

}

void FFCDelegateModule::NotifyUObjectDeleted(const class UObjectBase *InObject, int32 Index)
{
    // 这个如果开启了异步加载，就可能不是在主线程触发的
    if(IsInGameThread())
    {
        if(FCObjectUseFlag::GetIns().IsRef(InObject))
        {
            OnUObjectDeleteOnMainThread(InObject, Index);
        }
    }
    else
    {
        // 必要的话，可以加一个表，标记绑定了UObject对象的, 这个就可以检查，省掉一个异步
        if (FCObjectUseFlag::GetIns().IsRef(InObject))
        {
            AsyncTask(ENamedThreads::GameThread, 
            [this, InObject, Index]()
            {
                this->OnUObjectDeleteOnMainThread(InObject, Index);
            });
        }
    }
}

void FFCDelegateModule::OnUObjectDeleteOnMainThread(const class UObjectBase* InObject, int32 Index)
{
    FCObjectUseFlag::GetIns().UnRef(InObject);
    FCDynamicDelegateManager::GetIns().DeleteAllDelegateByUObject(InObject);
    FFCObjectdManager::GetSingleIns()->NotifyDeleteUObject(InObject, Index);
    FCGetObj::GetIns()->NotifyDeleteUObject(InObject, Index);

    if (CandidateInputComponents.Num() > 0)
    {
        int32 NumRemoved = CandidateInputComponents.Remove((UInputComponent*)InObject);
        if (NumRemoved > 0 && CandidateInputComponents.Num() < 1)
        {
            FWorldDelegates::OnWorldTickStart.Remove(OnWorldTickStartHandle);
        }
    }
}

//-------------------------------------------------------------

FString FFCDelegateModule::GetScriptPath()
{
    TSharedPtr<IPlugin> PlugIn = IPluginManager::Get().FindPlugin(TEXT("FCScript"));
    if(PlugIn.IsValid())
    {
        FString  ScriptPathName = PlugIn->GetContentDir();
		return ScriptPathName;
    }
    FString  ScriptPath = FPaths::ProjectContentDir();
	ScriptPath += TEXT("FCScript");

    return ScriptPath;
}

void FFCDelegateModule::Startup()
{
	bStartInit = true;
    
	// 加载脚本吧
	// 注册事件	
	if(!bAddUObjectNotify)
	{
		bAddUObjectNotify = true;
		GUObjectArray.AddUObjectCreateListener(this);    // add listener for creating UObject
		GUObjectArray.AddUObjectDeleteListener(this);    // add listener for deleting UObject
	}

    GetContextManger()->Init();
	FString  ScriptPath = GetScriptPath();
	LoadFCScript(GetClientScriptContext(), ScriptPath);

	lua_State* L = GetClientScriptContext()->m_LuaState;
	FCScriptDelegates::OnLuaStateCreateWrap.Broadcast(L);
	FCScriptDelegates::OnLuaStateStart.Broadcast(L);

	CallAnyScriptFunc(GetClientScriptContext(), 0, "OnGameStartup");	
}

void FFCDelegateModule::Shutdown()
{
	if(bStartInit)
	{
		CallAnyScriptFunc(GetClientScriptContext(), 0, "OnGameShutdown");
	}
	bStartInit = false;

	lua_State* L = GetClientScriptContext()->m_LuaState;
	FCScriptDelegates::OnLuaStateStart.Broadcast(L);

    FCDynamicDelegateManager::GetIns().Clear();
	FCGetObj::GetIns()->Clear();
	FCRefObjCache::GetIns()->Clear();
	ReleaseTempalteProperty();
    ClearAllDefaultValue();
	FFCObjectdManager::GetSingleIns()->Clear();
    if(L)
    {
        lua_gc(L, LUA_GCCOLLECT, 0);
        lua_gc(L, LUA_GCCOLLECT, 0);
    }
	GetContextManger()->Clear();
	GameInstance = nullptr;
	if(Ticker)
	{
		Ticker->RemoveFromRoot();
		Ticker = nullptr;
	}
    if(DelegateObject)
    {
        DelegateObject->RemoveFromRoot();
        DelegateObject = nullptr;
    }
	if(bAddUObjectNotify)
	{
		bAddUObjectNotify = false;
		GUObjectArray.RemoveUObjectCreateListener(this);
		GUObjectArray.RemoveUObjectDeleteListener(this);
	}
	ReleasePropertyTable();
    mScriptNameMap.clear();

    CandidateInputComponents.Empty();
    FWorldDelegates::OnWorldTickStart.Remove(OnWorldTickStartHandle);
}

UFunction* FFCDelegateModule::GetScriptNameFunction(UClass* ObjClass)
{
    mScriptNameCS.Lock();
    CClassToFunctionScriptMap::iterator itFunction = mScriptNameMap.find(ObjClass);
    if(itFunction != mScriptNameMap.end())
    {
        mScriptNameCS.Unlock();
        return itFunction->second;
    }
    mScriptNameCS.Unlock();

    if (ObjClass->IsChildOf<UPackage>() || ObjClass->IsChildOf<UClass>())             // filter out UPackage and UClass
    {
        mScriptNameCS.Lock();
        mScriptNameMap[ObjClass] = nullptr;
        mScriptNameCS.Unlock();
        return nullptr;
    }
    UFunction* InterfaceFunc = ObjClass->FindFunctionByName(mName_GetScriptClassName);

    // 做UnLua升级是可以这样
#ifdef COMPATIBLE_UNLUA
    if (!InterfaceFunc)
        InterfaceFunc = ObjClass->FindFunctionByName(mName_GetModuleName); // 兼容UnLua
#endif
    mScriptNameCS.Lock();
    mScriptNameMap[ObjClass] = InterfaceFunc;
    mScriptNameCS.Unlock();
    return InterfaceFunc;
}

void  FFCDelegateModule::TryBindScript(const class UObjectBaseUtility *Object)
{
	if(!Object)
	{
		return ;
	}

    if (Object->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))           // filter out CDO and ArchetypeObjects
        return ;

    UClass *ObjClass = Object->GetClass();
    if (ObjClass->IsChildOf<UPackage>() || ObjClass->IsChildOf<UClass>())             // filter out UPackage and UClass
    {
        return;
    }

    UFunction* InterfaceFunc = GetScriptNameFunction(ObjClass);
    if(InterfaceFunc)
    {
        if (InterfaceFunc->GetNativeFunc() && IsInGameThread())
        {
            FString ScriptClassName;
            UObject* DefaultObject = ObjClass->GetDefaultObject();             // get CDO
            DefaultObject->UObject::ProcessEvent(InterfaceFunc, &ScriptClassName);   // force to invoke UObject::ProcessEvent(...)
            if (ScriptClassName.Len() > 0)
            {
                // 绑定一个UObject到脚本对象, 脚本的类名不可以为空串
                FFCObjectdManager::GetSingleIns()->BindScript(Object, ObjClass, ScriptClassName);
            }
        }
        else
        {
            // 如果是后台线程，或对象在异步加载中, 做延迟绑定
            if (IsAsyncLoading())
            {
                bool bIsUObject = ObjClass->IsChildOf<UObject>();
                if(bIsUObject)
                {
                    FScopeLock Lock(&CandidatesCS);
                    Candidates.Add((UObject*)Object);  // mark the UObject as a candidate
                }
            }
        }        
    }
    else
    {
        // 如果是脚本中动态绑定的那种
        if (IsInGameThread() && FFCObjectdManager::GetSingleIns()->IsDynamicBindClass(ObjClass))
        {
            FFCObjectdManager::GetSingleIns()->DynamicBind(Object, ObjClass);
        }
    }

	//static UClass *InterfaceClass = UFCScriptInterface::StaticClass();
	//if (!Object->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))           // filter out CDO and ArchetypeObjects
	//{
	//	UClass *Class = Object->GetClass();
	//	if (Class->IsChildOf<UPackage>() || Class->IsChildOf<UClass>())             // filter out UPackage and UClass
	//	{
	//		return ;
	//	}
 //       if (Class->ImplementsInterface(InterfaceClass))                             // static binding
 //       {
 //           UFunction *Func = Class->FindFunctionByName(FName("GetScriptClassName"));
 //           if (Func)
 //           {
 //               bool bIsActor = Class->IsChildOf<AActor>();
 //               bool bIsUObject = Class->IsChildOf<UObject>();
 //               if (Func->GetNativeFunc() && IsInGameThread())
 //               {
 //                   FString ScriptClassName;
 //                   UObject *DefaultObject = Class->GetDefaultObject();             // get CDO
 //                   DefaultObject->UObject::ProcessEvent(Func, &ScriptClassName);   // force to invoke UObject::ProcessEvent(...)
	//				if(ScriptClassName.Len() > 0)
	//				{
	//					// 绑定一个UObject到脚本对象, 脚本的类名不可以为空串
	//					FFCObjectdManager::GetSingleIns()->BindScript(Object, Class, ScriptClassName);
	//				}
 //               }
 //               else
 //               {
	//				// 如果是后台线程，或对象在异步加载中, 做延迟绑定
	//				if (IsAsyncLoading())
 //                   {
 //                       FScopeLock Lock(&CandidatesCS);
 //                       Candidates.Add((UObject*)Object);  // mark the UObject as a candidate
 //                   }
 //               }
 //           }
	//	}
	//	else
	//	{
	//		// 如果是脚本中动态绑定的那种
	//		if( IsInGameThread() && FFCObjectdManager::GetSingleIns()->IsDynamicBindClass(Class))
	//		{
	//			FFCObjectdManager::GetSingleIns()->DynamicBind(Object, Class);
	//		}
	//	}
	//}
}

#if OLD_UE_ENGINE == 0
void FFCDelegateModule::OnWorldTickStart(UWorld *World, ELevelTick TickType, float DeltaTime)
#else
void FFCDelegateModule::OnWorldTickStart(ELevelTick TickType, float DeltaTime)
#endif
{
    CandidateInputComponents.Empty();
    FWorldDelegates::OnWorldTickStart.Remove(OnWorldTickStartHandle);
}

//-------------------------------------------------------------