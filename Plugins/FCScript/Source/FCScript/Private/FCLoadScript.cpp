#include "FCLoadScript.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "FCRunTimeRegister.h"
#include "FCInitWrap.h"
#include "FCBrigeHelper.h"
#include "Stats/Stats.h"


DEFINE_LOG_CATEGORY(LogFCScript);

//#if STATS
//DEFINE_STAT(STAT_UnLua_Lua_Memory);
//DEFINE_STAT(STAT_UnLua_PersistentParamBuffer_Memory);
//DEFINE_STAT(STAT_UnLua_OutParmRec_Memory);
//#endif

void* LuaAllocator(void* ud, void* ptr, size_t osize, size_t nsize)
{
    if (nsize == 0)
    {
        FMemory::Free(ptr);
        return nullptr;
    }

    void* Buffer = nullptr;
    if (!ptr)
    {
        Buffer = FMemory::Malloc(nsize);
    }
    else
    {
        Buffer = FMemory::Realloc(ptr, nsize);
    }
    return Buffer;
}

bool  LoadFCScript(FCScriptContext *Context, const FString  &ScriptPath)
{
	if(Context->m_bInit)
	{
		return Context->m_LuaState != NULL;
	}
	Context->m_bInit = true;


#if PLATFORM_WINDOWS
    // 防止类似AppleProResMedia插件忘了恢复Dll查找目录
    // https://github.com/Tencent/UnLua/issues/534
    const auto Dir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Binaries/Win64"));
    FPlatformProcess::PushDllDirectory(*Dir);
    Context->m_LuaState = lua_newstate(LuaAllocator, nullptr);       // create main Lua thread
    FPlatformProcess::PopDllDirectory(*Dir);
#else
    Context->m_LuaState = lua_newstate(LuaAllocator, nullptr);       // create main Lua thread
#endif
    lua_State* L = Context->m_LuaState;
    luaL_openlibs(L);

	// 开始注册吧
	RunTimeRegisterScript(Context);

	// 静态注册
	FCInitWrap::Register(L);

	// 注册全局C++类
	FCExportedClass::RegisterAll(L);
	
	// 加载UE类注册信息
	return true;
}