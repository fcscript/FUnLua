#include "FCDynamicClassDesc.h"

//#if STATS
//
//DECLARE_STATS_GROUP(TEXT("UnLua"), STATGROUP_UnLua, STATCAT_Advanced);
//DECLARE_MEMORY_STAT_EXTERN(TEXT("Lua Memory"), STAT_UnLua_Lua_Memory, STATGROUP_UnLua, /*UNLUA_API*/);
//DECLARE_MEMORY_STAT_EXTERN(TEXT("Persistent Parameter Buffer Memory"), STAT_UnLua_PersistentParamBuffer_Memory, STATGROUP_UnLua, /*UNLUA_API*/);
//DECLARE_MEMORY_STAT_EXTERN(TEXT("OutParmRec Memory"), STAT_UnLua_OutParmRec_Memory, STATGROUP_UnLua, /*UNLUA_API*/);
//#endif

bool  LoadFCScript(FCScriptContext *Context, const FString  &ScriptPath);

