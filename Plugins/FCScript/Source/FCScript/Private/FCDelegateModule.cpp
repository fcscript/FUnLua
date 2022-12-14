#include "FCDelegateModule.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Templates/Casts.h"
#include "Logging/LogMacros.h"

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

	// ??????????????????Player????????????
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
	FFCObjectdManager::GetSingleIns()->NotifyDeleteUObject(InObject, Index);
	FCGetObj::GetIns()->NotifyDeleteUObject(InObject, Index);
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
    
	// ???????????????
	// ????????????	
	if(!bAddUObjectNotify)
	{
		bAddUObjectNotify = true;
		GUObjectArray.AddUObjectCreateListener(this);    // add listener for creating UObject
		GUObjectArray.AddUObjectDeleteListener(this);    // add listener for deleting UObject
	}
	
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

	FCGetObj::GetIns()->Clear();
	FCRefObjCache::GetIns()->Clear();
	ReleaseTempalteProperty();
    ClearAllDefaultValue();
	FFCObjectdManager::GetSingleIns()->Clear();
    lua_gc(L, LUA_GCCOLLECT, 0);
    lua_gc(L, LUA_GCCOLLECT, 0);
	GetContextManger()->Clear();
	GameInstance = nullptr;
	if(Ticker)
	{
		Ticker->RemoveFromRoot();
		Ticker = nullptr;
	}
	if(bAddUObjectNotify)
	{
		bAddUObjectNotify = false;
		GUObjectArray.RemoveUObjectCreateListener(this);
		GUObjectArray.RemoveUObjectDeleteListener(this);
	}
	ReleasePropertyTable();
}

void  FFCDelegateModule::TryBindScript(const class UObjectBaseUtility *Object)
{
	if(!Object)
	{
		return ;
	}

	static UClass *InterfaceClass = UFCScriptInterface::StaticClass();
	if (!Object->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))           // filter out CDO and ArchetypeObjects
	{
		UClass *Class = Object->GetClass();
		if (Class->IsChildOf<UPackage>() || Class->IsChildOf<UClass>())             // filter out UPackage and UClass
		{
			return ;
		}
        if (Class->ImplementsInterface(InterfaceClass))                             // static binding
        {
            UFunction *Func = Class->FindFunctionByName(FName("GetScriptClassName"));
            if (Func)
            {
                bool bIsActor = Class->IsChildOf<AActor>();
                bool bIsUObject = Class->IsChildOf<UObject>();
                if (Func->GetNativeFunc() && IsInGameThread())
                {
                    FString ScriptClassName;
                    UObject *DefaultObject = Class->GetDefaultObject();             // get CDO
                    DefaultObject->UObject::ProcessEvent(Func, &ScriptClassName);   // force to invoke UObject::ProcessEvent(...)
					if(ScriptClassName.Len() > 0)
					{
						// ????????????UObject???????????????, ?????????????????????????????????
						FFCObjectdManager::GetSingleIns()->BindScript(Object, Class, ScriptClassName);
					}
                }
                else
                {
					// ???????????????????????????????????????????????????, ???????????????
					if (IsAsyncLoading())
                    {
                        FScopeLock Lock(&CandidatesCS);
                        Candidates.Add((UObject*)Object);  // mark the UObject as a candidate
                    }
                }
            }
		}
		else
		{
			// ???????????????????????????????????????
			if( IsInGameThread() && FFCObjectdManager::GetSingleIns()->IsDynamicBindClass(Class))
			{
				FFCObjectdManager::GetSingleIns()->DynamicBind(Object, Class);
			}
		}
	}
}

#if OLD_UE_ENGINE == 0
void FFCDelegateModule::OnWorldTickStart(UWorld *World, ELevelTick TickType, float DeltaTime)
#else
void FFCDelegateModule::OnWorldTickStart(ELevelTick TickType, float DeltaTime)
#endif
{

}

//-------------------------------------------------------------