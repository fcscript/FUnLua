// Fill out your copyright notice in the Description page of Project Settings.

#include "FCTicker.h"
#include "FCDynamicClassDesc.h"
#include "FCObjectManager.h"
#include "FCRefObjCache.h"
#include "../LuaCore/CallLua.h"


void UFCTicker::BeginDestroy()
{
	UObject::BeginDestroy();
}

void UFCTicker::Tick(float DeltaTime) 
{
	FCScriptContext *ScriptContext = GetScriptContext();
	// 
	if (ScriptContext->m_LuaState)
	{
        lua_State *L = ScriptContext->m_LuaState;
        lua_pushcfunction(L, ReportLuaCallError);
		CallGlobalVoidLua(L, "MainTick", DeltaTime);
        lua_pop(L, 1);
	}
	FFCObjectdManager::GetSingleIns()->CheckGC();
	FCRefObjCache::GetIns()->CheckGC(DeltaTime);
}

bool UFCTicker::IsTickable() const 
{
	return (HasAnyFlags(RF_ClassDefaultObject) == false);
}

TStatId UFCTicker::GetStatId() const 
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFCTicker, STATGROUP_Tickables);
}

void UFCTicker::OnLatentActionCompleted(int32 LinkID)
{
    GetScriptContext()->ResumeThread(LinkID); // resume a coroutine
}