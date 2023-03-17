#pragma once
#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectArray.h"
#include "Engine/World.h"
#include "CoreUObject.h"
#include "FCTicker.h"
#include "FCDelegateObject.h"
#include "FCPropertyType.h"

// -- 这个类用于UE的全局委托的交互模块，负责UE的全局委托与脚本的沟通
// -- 需要注册一些全局的UE事件
// -- 注册游戏Play事件
// -- 注册游戏Stop事件
// -- 注册关卡地图加载事件
// -- 注册UObject创建事件, 用来自动绑定脚本对象的产生
// -- 注册UObject释放事件，用来断开脚本对象与UObject的关联

class FFCDelegateModule : public FUObjectArray::FUObjectCreateListener, public FUObjectArray::FUObjectDeleteListener
{
public:
	void OnStartupModule();
public:
	// 一些全局性事件

	void OnWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources);
	void OnPostWorldCleanup(UWorld *World, bool bSessionEnded, bool bCleanupResources);
	void OnPreWorldInitialization(UWorld *World, const UWorld::InitializationValues);
	void OnPostWorldInitialization(UWorld *World, const UWorld::InitializationValues);
    void OnInitGameContext();
	void OnPostEngineInit();
	void OnPreExit();
	void OnAsyncLoadingFlushUpdate();
	void OnCrash();
	void PreLoadMap(const FString &MapName);
	void PostLoadMapWithWorld(UWorld *World);
	void OnPostGarbageCollect();

#if WITH_EDITOR
	void PreBeginPIE(bool bIsSimulating);
	void BeginPIE(bool bIsSimulating);
	void PostPIEStarted(bool bIsSimulating);
	void PrePIEEnded(bool bIsSimulating);
	void EndPIE(bool bIsSimulating);

	void OnEndPlayMap();
#endif
public:
	// 对象创建与销毁事件
	virtual void NotifyUObjectCreated(const class UObjectBase *InObject, int32 Index) override;
	virtual void OnUObjectArrayShutdown() override;
	
	virtual void NotifyUObjectDeleted(const class UObjectBase *InObject, int32 Index) override;

    void OnUObjectDeleteOnMainThread(const class UObjectBase* InObject, int32 Index);
protected:
    FString GetScriptPath();
	void Startup();
	void Shutdown();
    UFunction *GetScriptNameFunction(UClass* ObjClass);
	void  TryBindScript(const class UObjectBaseUtility *Object);

#if OLD_UE_ENGINE == 0
	void OnWorldTickStart(UWorld *World, ELevelTick TickType, float DeltaTime);
#else
	void OnWorldTickStart(ELevelTick TickType, float DeltaTime);
#endif
    void OnMapLoaded(UWorld* World);
protected:
	FDelegateHandle OnActorSpawnedHandle;
	FDelegateHandle OnWorldTickStartHandle;
	FDelegateHandle OnPostGarbageCollectHandle;

    struct FActorInputComponentCache
    {
        UObject  *Object;
        class UInputComponent*  InputComponent;
    };

	TArray<FActorInputComponentCache> CandidateInputComponents;

	TArray<FWeakObjectPtr> Candidates;        // binding candidates during async loading
	FCriticalSection CandidatesCS;      // critical section for accessing 'Candidates'
	UFCTicker  *Ticker = nullptr;
    UFCDelegateObject *DelegateObject = nullptr;
	UGameInstance *GameInstance = nullptr;
	bool        bStartInit = false;
	bool        bAddUObjectNotify = false;

    typedef  std::unordered_map<UClass*, UFunction*>   CClassToFunctionScriptMap; // UClass == > UFunction
    CClassToFunctionScriptMap   mScriptNameMap;
    FCriticalSection            mScriptNameCS;
    FName   mName_GetScriptClassName = "GetScriptClassName";
    FName   mName_GetModuleName = "GetModuleName";
};
