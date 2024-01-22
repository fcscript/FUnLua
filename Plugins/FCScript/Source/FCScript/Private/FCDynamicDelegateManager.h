
#pragma once
#include "FCDynamicClassDesc.h"

struct FCLuaDelegateKey
{
    const void* TableOrObjectPtr;
    const void* LuaFuncAddr;
    FCLuaDelegateKey() :TableOrObjectPtr(nullptr), LuaFuncAddr(nullptr) {}
    FCLuaDelegateKey(const void* InTableOrObjectPtr, const void* InLuaFuncAddr) :TableOrObjectPtr(InTableOrObjectPtr), LuaFuncAddr(InLuaFuncAddr) {}
};

struct FCLuaDelegate : public FCDelegateInfo
{
    UClass* OuterClass;
    UFunction* Function;
    FScriptDelegate  Delegate;
    UObject *Object; // 脚本中的来源对象
    UObject *Outer;  // 当前的Outer对象
    int   ScriptIns;
    bool  bNoneCallByZeroParam;

    FCLuaDelegateKey  m_Key;
    FCLuaDelegate():OuterClass(nullptr), Function(nullptr), Object(nullptr), Outer(nullptr), ScriptIns(0), bNoneCallByZeroParam(false)
    {
    }
};

struct FCLuaDelegateListNode
{
    FCLuaDelegateListNode *m_pLast;
    FCLuaDelegateListNode *m_pNext;
    FCLuaDelegate         *m_Delegate;
    FCLuaDelegateListNode():m_pLast(nullptr), m_pNext(nullptr), m_Delegate(nullptr){}
};

typedef CFastList<FCLuaDelegateListNode>   FCLuaDelegateList;

template<> struct std::hash<FCLuaDelegateKey>
{
    size_t operator()(const FCLuaDelegateKey& Key) const
    {
        return (size_t)Key.TableOrObjectPtr + (size_t)Key.LuaFuncAddr;
    }
};
template<> struct std::equal_to<FCLuaDelegateKey>
{
    bool operator()(const FCLuaDelegateKey& key1, const FCLuaDelegateKey& key2) const
    {
        return key1.TableOrObjectPtr == key2.TableOrObjectPtr
            && key1.LuaFuncAddr == key2.LuaFuncAddr;
    }
};

typedef  std::unordered_map<const void*, FCDynamicOverrideFunction*>   CAdr2DynamicFuncMap; // void* ==> FCDynamicFunction
typedef  std::unordered_map<const void*, FCDynamicProperty*>   CAdr2DynamicPropertyMap; // void* ==> FCDynamicProperty
typedef  std::unordered_map<const void*, bool>   CAdr2FlagsMap; // void* ==> bool

typedef  std::unordered_map<FCLuaDelegateKey, FCLuaDelegate*>  CLuaAddr2DelegateMap; // LuaUncAddr ==> FCLuaDelegate
typedef  std::unordered_map<UFunction*, FCLuaDelegateList>   CUFunction2DelegateListMap; // UFunction* ==> FCLuaDelegateList
typedef  std::unordered_map<const UObjectBase*, FCLuaDelegateList>   CUObject2DelegateListMap; // UObject* ==> FCLuaDelegateList

class FCDynamicDelegateManager
{
public:
    static FCDynamicDelegateManager &GetIns();
public:
    // 功能：生成一个Lua函数的委托对象(如果没有Object对象，就是纯Lua函数)
    FCLuaDelegate* MakeLuaDelegate(UObject* Object, UObject* Outer, lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty);
    FCLuaDelegate* MakeDelegateByTableParam(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty);
    FCLuaDelegate* MakeDelegate(lua_State* L, const FCDelegateInfo* InDelegateInfo, const FCDynamicPropertyBase* DynamicProperty);

    UObject* OverridenLuaFunction(UObject* Object, UObject* Outer, lua_State* L, int ScriptIns, UFunction* ActionFunc, const FName& FuncName, bool bNoneCallByZeroParam);

    // 功能：删除所有该对象下挂载的委托对象
    void   DeleteAllDelegateByUObject(const UObjectBase*Object);

    void   Clear();
public:
    FCLuaDelegateList *FindDelegateByFunction(UFunction *Function)
    {
        CUFunction2DelegateListMap::iterator itDelegate = m_UFunction2DelegateListMap.find(Function);
        if(itDelegate != m_UFunction2DelegateListMap.end())
            return &(itDelegate->second);
        else
            return nullptr;
    }
protected:
    void  AddBindLuaFunction(FCLuaDelegate *Delegate, UObject* Object);
    FCDynamicOverrideFunction *GetDynamicFunction(UFunction* Function);
    FCDynamicProperty  *GetDynamicProperty(const FProperty* InProperty, const char* InName = nullptr);

    void  DeleteDelegateList(FCLuaDelegateList& DelegateList);
    void  DeleteLuaDelegate(FCLuaDelegate* Delegate);
    void  MakeScriptDelegate(FCLuaDelegate *Delegate, const void *LuaFuncAddr, UObject* Outer, const FCDynamicPropertyBase* DynamicProperty);
    // 生成一个替换的接口
    // SrcFunction - 来源函数
    // OuterClass - 来源UClass
    // NewFuncName - 新的函数名
    UFunction *  MakeReplaceFunction(UFunction *SrcFunction, UClass* OuterClass, const FName &NewFuncName, FNativeFuncPtr InFunc);

    FCLuaDelegateListNode  *NewDelegateListNode();
    void DelDelgateListNode(FCLuaDelegateListNode *pNode);
protected:
    CLuaAddr2DelegateMap  m_LuaAddr2DelegateMap;  // LuaUncAddr ==> FCLuaDelegate
    CUFunction2DelegateListMap  m_UFunction2DelegateListMap;  // UFunction* ==> FCLuaDelegateList
    CUObject2DelegateListMap m_UObject2DelegateListMap;  // UObject* ==> FCLuaDelegateList


    CAdr2DynamicFuncMap m_DynamicFuncMap;
    CAdr2DynamicPropertyMap m_DynamicProperyMap;

    CAdr2FlagsMap  m_FuncLinkFlagMap;  // 函数Link标记(不可释放)
};

void FCDynamicDelegate_CallLua(UObject* Context, FFrame& TheStack, RESULT_DECL);