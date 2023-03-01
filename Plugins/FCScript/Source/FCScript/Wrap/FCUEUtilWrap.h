#pragma once
#include "FCBrigeHelper.h"

class FCUEUtilWrap
{
public:	
    static void Register(lua_State* L);
    static void RegisterBaseType(lua_State* L);

    static int LibOpen_wrap(lua_State* L);
    static int LibOpenBaseType_wrap(lua_State* L);
    static int LibOverriden_wrap(lua_State* L);
    static int Overriden_Index(lua_State* L);

    static int DoGetName_wrap(lua_State* L, void* ObjRefPtr, UObject *ThisObject);
    static int DoGetWorld_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetClass_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetParent_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetBPObject_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetObjectAddr_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetObjectRefID_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoOverridden_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetClassDesc_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoGetStaticClass_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoCast_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoAddGCRef_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoReleaseGCRef_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoSpawActor_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
    static int DoLoad_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);

	static int NewObject_wrap(lua_State* L);
    static int SpawActor_wrap(lua_State* L);
    static int LoadObject_wrap(lua_State* L);
    static int LoadClass_wrap(lua_State* L);
    static int LoadUserWidget_wrap(lua_State* L);
    static int GetBindObjectCount_wrap(lua_State* L);
    static int GetTotalObjRef_wrap(lua_State* L);
    static int GetTotalIntPtr_wrap(lua_State* L);
    static int GetObjRefSize_wrap(lua_State* L);
    static int GetClassDescMemSize_wrap(lua_State* L);
    static int ListGCObjects_wrap(lua_State* L);
    static int PrintGCObjects_wrap(lua_State* L);
};