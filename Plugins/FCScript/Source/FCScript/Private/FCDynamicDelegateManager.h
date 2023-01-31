
#pragma once
#include "FCDynamicClassDesc.h"

struct FCLuaDelegate : public FCDelegateInfo
{
    FCLuaDelegate* m_pNext;  // 同一个UObect下面的
    UClass* OuterClass;
    UFunction* Function;
    FScriptDelegate  Delegate;
    UObject *Object; // 脚本中的来源对象
    UObject *Outer;  // 当前的Outer对象
    int   ObjRefID;
    int   ScriptIns;
    FCLuaDelegate():m_pNext(nullptr), OuterClass(nullptr), Function(nullptr), Object(nullptr), Outer(nullptr), ObjRefID(0), ScriptIns(0)
    {
    }
};

typedef  std::unordered_map<const void*, FCLuaDelegate*>   CAdr2DelegateMap; // void* ==> FCLuaDelegate
typedef  std::unordered_map<const void*, FCDynamicOverrideFunction*>   CAdr2DynamicFuncMap; // void* ==> FCDynamicFunction
typedef  std::unordered_map<const void*, FCDynamicProperty*>   CAdr2DynamicPropertyMap; // void* ==> FCDynamicProperty
typedef  std::unordered_map<const void*, bool>   CAdr2FlagsMap; // void* ==> bool

class FCDynamicDelegateManager
{
public:
    static FCDynamicDelegateManager &GetIns();
public:
    // 功能：生成一个Lua函数的委托对象(如果没有Object对象，就是纯Lua函数)
    int64  MakeLuaDelegateRefObj(UObject * Object, lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty);

    // 功能：生成一个Lua函数的委托对象(如果没有Object对象，就是纯Lua函数)
    FCLuaDelegate* MakeLuaDelegate(UObject* Object, lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty);
    // 功能：通过函数地址一个委托对象
    void   DeleteLuaDelegateByFuncAddr(const void* LuaFuncAddr);

    // 功能：删除所有该对象下挂载的委托对象
    void   DeleteAllDelegateByUObject(const UObjectBase*Object);

    void   Clear();

public:
    FCLuaDelegate *FindDelegateByFunction(UFunction *Function)
    {
        CAdr2DelegateMap::iterator itDelegate = m_UEFuncAddr2DelegateMap.find(Function);
        if(itDelegate != m_UEFuncAddr2DelegateMap.end())
            return itDelegate->second;
        else
            return nullptr;
    }
protected:
    FCDynamicOverrideFunction *GetDynamicFunction(UFunction* Function);
    FCDynamicProperty  *GetDynamicProperty(const FProperty* InProperty, const char* InName = nullptr);

    void  EraseDelegatePtr(CAdr2DelegateMap &PtrMap, FCLuaDelegate* Delegate, const void *Key);

    void  DeleteDelegateList(FCLuaDelegate* Delegate);
    void  DeleteLuaDelegate(FCLuaDelegate* Delegate);
    void  MakeScriptDelegate(FCLuaDelegate *Delegate, const void *LuaFuncAddr, UObject* Outer, const FCDynamicPropertyBase* DynamicProperty);
protected:
    CAdr2DelegateMap   m_FuncAdr2DelegateMap;
    CAdr2DelegateMap   m_Object2ChildMap;
    CAdr2DelegateMap   m_UEFuncAddr2DelegateMap;

    CAdr2DynamicFuncMap m_DynamicFuncMap;
    CAdr2DynamicPropertyMap m_DynamicProperyMap;

    CAdr2FlagsMap  m_FuncLinkFlagMap;  // 函数Link标记(不可释放)
};

void FCDynamicDelegate_CallLua(UObject* Context, FFrame& TheStack, RESULT_DECL);