#pragma once
#include "FCDynamicClassDesc.h"

// -- 脚本对象与UObject对象的关联管理器
struct FBindObjectInfo
{
	const UObjectBaseUtility  *Object;
	int32 Index;
	int64    m_ScriptIns;
	int64    m_ObjRefID;
    int      m_InitializerTableRef;
	const char*  m_ScriptName;
	FBindObjectInfo() :Object(nullptr), Index(0), m_ScriptIns(0), m_ObjRefID(0), m_InitializerTableRef(LUA_NOREF), m_ScriptName(nullptr)
	{
	}
	FBindObjectInfo(const UObjectBaseUtility *InObject, int32 InIndex, const char *InScriptName, int InitializerTableRef = LUA_NOREF) :Object(InObject), Index(InIndex), m_ScriptIns(0), m_ObjRefID(0), m_InitializerTableRef(InitializerTableRef), m_ScriptName(InScriptName)
	{
	}
	void  Set(const UObjectBaseUtility *InObject, int32 InIndex, const char *InScriptName, int InitializerTableRef)
	{
		Object = InObject;
		Index = InIndex;
		m_ScriptIns = 0;
		m_ScriptName = InScriptName;
        m_InitializerTableRef = InitializerTableRef;
	}
};

struct FScriptOverrideKey
{
    const UObjectBase*   Object;    // 绑定的对象
    UFunction* Function;  // 绑定的函数
    FScriptOverrideKey() :Object(nullptr), Function(nullptr) {}
    FScriptOverrideKey(const UObjectBase *InObject, UFunction *InFunction):Object(InObject), Function(InFunction){}
};

template<> struct std::hash<FScriptOverrideKey>
{
    size_t operator()(const FScriptOverrideKey& Key) const
    {
        return size_t(Key.Object) + size_t(Key.Function);
    }
};

template<> struct std::equal_to<FScriptOverrideKey>
{
    bool operator()(const FScriptOverrideKey& key1, const FScriptOverrideKey& key2) const
    {
        return key1.Object == key2.Object
            && key1.Function == key2.Function;
    }
};

class FFCObjectdManager
{
public:
	FFCObjectdManager();
	~FFCObjectdManager();
public:
	static FFCObjectdManager  *GetSingleIns();
public:
	void  Clear();

	void  BindScript(const class UObjectBaseUtility* Object, UClass* Class, const FString& ScriptClassName);
	void  BindToScript(const class UObjectBaseUtility* Object, UClass* Class, const char *ScriptClassName);
    void  CallBindScript(UObject *InObject, const char *ScriptClassName, int InitializerTableRef = LUA_NOREF);
	void  DynamicBind(const class UObjectBaseUtility *Object, UClass *Class);

	// 功能：对象释放事件
	void  NotifyDeleteUObject(const class UObjectBase *Object, int32 Index);

	void  PushDynamicBindClass(UClass *Class, const char *ScriptClassName);
	void  PopDynamicBindClass();
	// 返回是不是当前动态绑定的对象
	bool  IsDynamicBindClass(UClass *Class);

    void  OnBindScript(int64 ObjID, int64 ScriptIns)
    {
        m_BindScriptInsMap[ObjID] = ScriptIns;
    }
    bool  GetBindScriptIns(int64 ObjID, int64 &OutScriptIns) const
    {        
        CBindScriptInsMap::const_iterator itFind = m_BindScriptInsMap.find(ObjID);
        if(itFind != m_BindScriptInsMap.end())
        {
            OutScriptIns = itFind->second;
            return true;
        }
        return false;
    }
	bool  IsBindScript(const class UObjectBaseUtility *Object) const
	{
        CBindObjectInfoMap::const_iterator itFind = m_BindObjects.find(Object);
        return itFind != m_BindObjects.end();
	}
	FBindObjectInfo*  FindBindObjectInfo(const class UObjectBaseUtility *Object)
	{
		CBindObjectInfoMap::iterator itFind = m_BindObjects.find(Object);
		if(itFind != m_BindObjects.end())
		{
			return &(itFind->second);
		}
		return nullptr;
	}	
	int  GetBindObjectCount() const
	{
		return (int)m_BindObjects.size();
	}
public:
	FCDynamicOverrideFunction*RegisterReceiveBeginPlayFunction(UObject *InObject, UClass* Class);

	// 功能：覆盖函数
	FCDynamicOverrideFunction *RegisterOverrideFunc(UObject *InObject, int64 InScriptID, const char *InFuncName);
	FCDynamicOverrideFunction *ToOverrideFunction(UObject *InObject, UFunction *InFunction, FNativeFuncPtr InFuncPtr, int InNativeBytecodeIndex);
	FCDynamicOverrideFunction *FindOverrideFunction(UObject *InObject, UFunction *InFunction);
    int64 FindOverrideScriptIns(UObject *InObject, UFunction *InFunction);

	// 执行原生的调用
	int  NativeCall(UObject* InObject, FCDynamicFunction* DynamicFunc, lua_State* L, int nStart);

	FCDynamicDelegateList  *FindDelegateFunction(UObject *InObject);
	void  RegisterScriptDelegate(UObject *InObject, const FCDynamicProperty* InDynamicProperty, const void *InFuncAddr, int InFunctionRef, const int *InParams, int InParamCount);
	void  RemoveScriptDelegate(UObject *InObject, const FCDynamicProperty* InDynamicProperty, const void* InFuncAddr);
	void  ClearScriptDelegate(UObject* InObject, const FCDynamicProperty* InDynamicProperty);

	void  CheckGC();
protected:
	void  ClearObjectDelegate(const class UObjectBase *Object);
	void  ClearAllDynamicFunction();
	void  AddDelegateToClass(FCDynamicOverrideFunction *InDynamicFunc, UClass *InClass);
	void  RemoveDelegateFromClass(FCDynamicOverrideFunction *InDynamicFunc, UClass *InClass);
	void  RemoveObjectDelegate(UObject *InObject, const FCDynamicProperty* InDynamicProperty);
    void  RemoveOverrideRefByObject(const class UObjectBase *Object);
protected:
	struct FDynmicBindClassInfo
	{
		UClass  *Class;
		const char *ScriptClassName;
	};
	struct FBindReceiveBeginPlayInfo
	{
		int  Ref;
		FNativeFuncPtr  NativeFuncPtr;
		FBindReceiveBeginPlayInfo():Ref(0), NativeFuncPtr(nullptr){}
	};
	typedef  std::unordered_map<const UObjectBase*, FBindObjectInfo>   CBindObjectInfoMap;
	typedef  std::unordered_map<const UClass*, FBindReceiveBeginPlayInfo>   CBindReceiveBeginPlayRefMap;
    typedef  std::unordered_map<int64, int64>   CBindScriptInsMap;

	std::vector<FDynmicBindClassInfo>   m_DynamicBindClassInfo;
	UClass*             m_pCurrentBindClass;
	const char *        m_ScriptsClassName;
 
	CBindObjectInfoMap  m_BindObjects;
    CBindScriptInsMap  m_BindScriptInsMap;

	// ------------------------------	
	typedef std::unordered_map<UFunction*, FCDynamicOverrideFunction*> COverrideFunctionMap;
	typedef std::unordered_map<UObject *, FCDynamicDelegateList>   CObjectDelegateMap;
	typedef std::unordered_map<UFunction*, int>                    CFunctionRefMap;

	COverrideFunctionMap         m_OverrideFunctionMap;  // OverrideFunction
	CObjectDelegateMap           m_ObjectDelegateMap;    // 对象委托列表	
	CFunctionRefMap              m_DelegateRefMap;       // 委托引用计数
    // ------------------------------	

    // ------------------------------	函数重载记录 -------------------------
    typedef std::vector<UFunction*>  CScriptFunctionList;
    typedef std::unordered_map<FScriptOverrideKey, int64> COverrideFunction2ScriptInsMap;
    typedef std::unordered_map<UObjectBase*, CScriptFunctionList> COverrideObjectFunctionMap;
    COverrideFunction2ScriptInsMap   m_OverrideFunctionScriptInsMap;  // 重载的脚本实例
    COverrideObjectFunctionMap       m_OverrideObjectFunctionMap;    //  
    CFunctionRefMap                  m_OverrideRefMap;
    // ------------------------------	

	//-----------------延迟ReceiveBeginPlay
	// 
	typedef std::vector<UObject*>     CDelayCallBeginPlayList;
	CDelayCallBeginPlayList        m_DelayCallBeginPlayList;

	//-------------------------------------
};