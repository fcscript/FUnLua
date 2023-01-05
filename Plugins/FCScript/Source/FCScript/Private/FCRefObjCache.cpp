#include "FCRefObjCache.h"
#include "FCDynamicClassDesc.h"
#include "FCGetObj.h"
#include "FCSetArg.h"

FCRefObjCache::FCRefObjCache():m_fPassTime(0)
	, m_UnitReleaseCount(1)
{
}

FCRefObjCache::~FCRefObjCache()
{
}

FCRefObjCache* FCRefObjCache::GetIns()
{
	static FCRefObjCache   SRefObjCache;
	return &SRefObjCache;
}

template <class _TyList>
void  ReleaseCacheList(_TyList &CacheList)
{
	while (CacheList.size() > 0)
	{
		auto  first = CacheList.front_ptr();
		CacheList.pop_front();
		delete first;
	}
}

void  FCRefObjCache::Clear()
{
	// 释放内存吧
	m_ID2CacheMap.clear();
	ReleaseCacheList(m_CacheList);
	ReleaseCacheList(m_InvalidList);
}

void  FCRefObjCache::CheckGC(float DeltaTime)
{
	// 按先入先出的释放，每秒释放1/4, 每帧释放1/240, 就按1/100的释放吧
	m_fPassTime += DeltaTime;
	if (m_fPassTime > 1)
	{
		m_fPassTime = 0;
		m_UnitReleaseCount = m_CacheList.size() / 100;
		if (m_UnitReleaseCount < 1)
		{
			m_UnitReleaseCount = 1;
		}
	}
	int nCount = m_UnitReleaseCount;
	if (nCount > m_CacheList.size())
	{
		nCount = m_CacheList.size();
	}
	FCScriptContext* ScriptContext = GetScriptContext();
	lua_State *L = ScriptContext->m_LuaState;
	FCGetObj* ObjMng = FCGetObj::GetIns();

	for(;nCount>0; --nCount)
	{
		FRefCacheInfo* first = m_CacheList.front_ptr();
		if (first)
		{
			m_CacheList.pop_front();
			luaL_unref(L, LUA_REGISTRYINDEX, first->LuaRef);

			if(first->Ref > 0)
				ObjMng->ReleaseCacheRef(first->ObjID, first->Ref);
			m_ID2CacheMap.erase(first->ObjID);

			first->m_pLast = nullptr;
			first->m_pNext = nullptr;
			m_InvalidList.push_back(first);
		}
	}
}

void  FCRefObjCache::PushBindLuaValue(lua_State* L, int64 ObjID, const char* ClassName)
{
	CRefID2LuaCacheMap::iterator itFind = m_ID2CacheMap.find(ObjID);
	if (itFind != m_ID2CacheMap.end())
	{
		FRefCacheInfo* CacheInfo = itFind->second;
		CacheInfo->Ref++;
		lua_rawgeti(L, LUA_REGISTRYINDEX, CacheInfo->LuaRef);

		m_CacheList.erase(m_CacheList.MakeIterator(CacheInfo));
		CacheInfo->m_pLast = nullptr;
		CacheInfo->m_pNext = nullptr;
		m_CacheList.push_back(CacheInfo);
		return;
	}

    // 如果是内部的类，比如TArray这种，就不要再注册了
    if(!IsWrapClassName(ClassName))
        GlbRegisterClass(L, ClassName);

	FRefCacheInfo* CacheInfo = m_InvalidList.front_ptr();
	if (CacheInfo)
	{
		m_InvalidList.pop_front();
		CacheInfo->m_pLast = nullptr;
		CacheInfo->m_pNext = nullptr;
	}
	else
	{
		CacheInfo = new FRefCacheInfo();
	}
	CacheInfo->ObjID = ObjID;
	CacheInfo->Ref = 0;
	CacheInfo->LuaRef = -1;

	void* addr = lua_newuserdata(L, sizeof(int64*));
	*((int64*)addr) = ObjID;

	luaL_getmetatable(L, ClassName);
	lua_setmetatable(L, -2);

	lua_pushvalue(L, -1);
	CacheInfo->LuaRef = luaL_ref(L, LUA_REGISTRYINDEX);  // 将这个参数添加到全局引用表

	m_CacheList.push_back(CacheInfo);
	m_ID2CacheMap[ObjID] = CacheInfo;
}