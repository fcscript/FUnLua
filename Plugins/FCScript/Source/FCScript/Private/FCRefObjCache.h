#pragma once

#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject.h"

#include "FCStringCore.h"
#include "../LuaCore/LuaHeader.h"

class FCRefObjCache
{
	struct FRefCacheInfo
	{
		FRefCacheInfo* m_pLast;
		FRefCacheInfo* m_pNext;

		int64  ObjID;   // 对象ID
		int    LuaRef;  // lua 引用计数
		int    Ref;     // Cpp 引用计数
		FRefCacheInfo() :m_pLast(nullptr), m_pNext(nullptr), ObjID(0), LuaRef(-1), Ref(0) {}
	};
	typedef  std::unordered_map<int64, FRefCacheInfo*>   CRefID2LuaCacheMap;  // RefID ==> RefCacheInfo

	CRefID2LuaCacheMap        m_ID2CacheMap;  // ID ==> RefCacheInfo
	CFastList<FRefCacheInfo>  m_CacheList;    // 缓存列表
	CFastList<FRefCacheInfo>  m_InvalidList;  // 无效的回收列表
	float                     m_fPassTime;
	int                       m_UnitReleaseCount;
public:
	FCRefObjCache();
	~FCRefObjCache();

	static FCRefObjCache* GetIns();
public:
	void  Clear();
	void  CheckGC(float DeltaTime);
    int  OnDestoryObjRef(int64 ObjID);
	void  PushBindLuaValue(lua_State* L, int64 ObjID, const char* ClassName);
};
