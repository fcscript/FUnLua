#pragma once
#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "CoreUObject.h"
#include "FCPropertyType.h"
#include "FCStringCore.h"
#include "FCStringBuffer.h"
#include "../LuaCore/LuaHeader.h"
#include "FCObjectReferencer.h"
#include "FCObjectUseFlag.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFCScript, Log, All);

struct FCDynamicField
{
	virtual ~FCDynamicField() {}
	virtual int DoGet(lua_State* L, void* ObjRefPtr, void *ClassDescPtr) { return 0; }
	virtual int DoSet(lua_State* L, void* ObjRefPtr, void *ClassDescPtr) { return 0; }
	virtual const char* GetFieldName() const { return ""; }
	virtual FCDynamicField* Clone() const { return nullptr; }
    virtual int GetMemSize() const{ return sizeof(FCDynamicField);}
};

// 动态属性(反射)
struct FCDynamicPropertyBase : public FCDynamicField
{
	int		ElementSize;      // 元素所点字节
	int		Offset_Internal;  // 相对偏移
	int     PropertyIndex;    // 参数或属性索引(序号)
	const char* Name;        // 变量名,调试时用的(常引用)
    const char* ClassName;   // 类名
	
	FCPropertyType    Type;       // 类型
	const FProperty  *Property;
	bool              bRef;       // 是不是引用类型
	bool              bOuter;     // 是不是输出类型
    bool              bTempNeedRef;  // 临时的上下拷贝参数标记
    bool              bTempRealRef;  // 
	
	FCDynamicPropertyBase() :ElementSize(0), Offset_Internal(0), PropertyIndex(0), Name(nullptr), ClassName(nullptr), Type(FCPropertyType::FCPROPERTY_Unkonw), Property(nullptr), bRef(false), bOuter(false), bTempNeedRef(false), bTempRealRef(false)
	{
	}
	bool  IsRef() const
	{
		return bRef;
	}
	bool  IsOuter() const
	{
		return bOuter;
	}
	const char* GetFieldName() const
	{
		return Name;
	}
	const char* GetClassName() const
	{
		return ClassName;
	}
	// 功能：得到委托的触发函数
	UFunction *GetSignatureFunction() const
	{		
		if(FCPropertyType::FCPROPERTY_MulticastDelegateProperty == Type)
		{
			FMulticastDelegateProperty* DelegateProperty = (FMulticastDelegateProperty*)Property;
			return DelegateProperty->SignatureFunction;
		}
		else if(FCPROPERTY_DelegateProperty == Type)
		{
			FDelegateProperty* DelegateProperty = (FDelegateProperty*)Property;
			return DelegateProperty->SignatureFunction;
		}
        else if(FCPROPERTY_MulticastSparseDelegateProperty == Type)
        {
            FMulticastSparseDelegateProperty * DelagateProperty = (FMulticastSparseDelegateProperty*)Property;
            return DelagateProperty->SignatureFunction;
        }
		return nullptr;
	}
    virtual int GetMemSize() const { return sizeof(FCDynamicPropertyBase); }
};

typedef  void(*LPPushScriptValueFunc)(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void *ObjRefPtr);
typedef  void(*LPOuterScriptValueFunc)(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr);
typedef  bool(*LPCopyScriptValueFunc)(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr);

// 动态属性(反射)
struct FCDynamicProperty : public FCDynamicPropertyBase
{
	LPPushScriptValueFunc    m_WriteScriptFunc;   // 将UE变量写入到脚本
	LPOuterScriptValueFunc   m_ReadScriptFunc;    // 将脚本变量写入到UE对象
    LPCopyScriptValueFunc    m_CopyScriptValue;   // 将数据拷贝批指定的脚本变量

	FCDynamicProperty():m_WriteScriptFunc(nullptr), m_ReadScriptFunc(nullptr), m_CopyScriptValue(nullptr)
	{
	}

	void  InitProperty(const FProperty *InProperty, const char *InName = nullptr);
    void  InitCppType(FCPropertyType InType, const char *InClassName, int InElementSize);
	virtual FCDynamicField* Clone() const { return new FCDynamicProperty(*this); }
	virtual int DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
	virtual int DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
    virtual int GetMemSize() const { return sizeof(FCDynamicProperty); }
};

struct FCDefaultParamBase
{
    virtual ~FCDefaultParamBase() {}

    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char *DefaultParam2) = 0;
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr) = 0;
};

struct FCDynamicFunctionParam : public FCDynamicProperty
{
    FCDynamicFunctionParam():DefaultParam(nullptr)
    {
    }
    FCDefaultParamBase *DefaultParam;
};

struct  FCDynamicFunction : public FCDynamicField
{
	UFunction  *Function;
    int     LatentPropertyIndex;
	int     ReturnPropertyIndex;
	int     ParmsSize;        // 参数序列化后的字节大小
	int     ParamCount;       // 函数参数个数(不包括返回值)
    int     OuterParamCount;  // 
    int     OuterParamSize;
	bool    bOverride;
	bool    bOuter;
	bool    bRegister;        // 是不是在类中注册了
	bool    bDelegate;
	const char* Name;        // 函数名
	std::vector<FCDynamicFunctionParam>   m_Property;
	FCDynamicFunction():Function(nullptr), LatentPropertyIndex(-1), ReturnPropertyIndex(-1), ParmsSize(0), ParamCount(0), OuterParamCount(0), OuterParamSize(0), bOverride(false), bOuter(false), bRegister(false), bDelegate(false), Name(nullptr)
	{
	}
	void  InitParam(UFunction *InFunction);
    bool IsLatentFunction() const 
    {
        return LatentPropertyIndex != -1; 
    }
	const char* GetFieldName() const
	{
		return Name;
	}
	virtual FCDynamicField* Clone() const { return new FCDynamicFunction(*this); }
	virtual int DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
	virtual int DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
    virtual int GetMemSize() const { return sizeof(FCDynamicFunction); }
};

struct FCDynamicOverrideFunction : public FCDynamicFunction
{
	FNativeFuncPtr   OleNativeFuncPtr;  // 原始的NativeFunc函数
	FNativeFuncPtr   CurOverrideFuncPtr;
    const char*      LuaFunctionMame; // Lua端的函数名(RPC)
	int              m_NativeBytecodeIndex;
	bool             m_bLockCall;
    bool             m_bNeedRestoreNative;
	TArray<uint8>    m_NativeScript;
	FCDynamicOverrideFunction() : OleNativeFuncPtr(nullptr), CurOverrideFuncPtr(nullptr), LuaFunctionMame(nullptr), m_NativeBytecodeIndex(0), m_bLockCall(false), m_bNeedRestoreNative(false)
	{
	}
    const char *GetLuaFunctionName() const
    {
        return LuaFunctionMame ? LuaFunctionMame : Name;
    }
    virtual int GetMemSize() const { return sizeof(FCDynamicOverrideFunction); }
};

typedef int (*LPWrapLibFunction)(lua_State* L, void *ObjRefPtr, UObject *ThisObject);
typedef int (*LPLuaLibOpenCallback)(lua_State* L);

struct FCDynamicWrapField : public FCDynamicField
{
	const char * Name;        // 函数名或属性名
	LPWrapLibFunction   m_GetFunction;
	LPWrapLibFunction   m_SetFunction;
	FCDynamicWrapField():m_GetFunction(nullptr), m_SetFunction(nullptr) {}
	FCDynamicWrapField(LPWrapLibFunction InGetFunc, LPWrapLibFunction InSetFunc) :m_GetFunction(InGetFunc), m_SetFunction(InSetFunc) {}
	const char* GetFieldName() const
	{
		return Name;
	}
    virtual int GetMemSize() const { return sizeof(FCDynamicWrapField); }
};

struct FCDynamicWrapLibFunction : public FCDynamicFunction
{
	LPWrapLibFunction   m_GetFunction;
	LPWrapLibFunction   m_SetFunction;
	FCDynamicWrapLibFunction() :m_GetFunction(nullptr), m_SetFunction(nullptr) {}
	FCDynamicWrapLibFunction(LPWrapLibFunction InGetFunc, LPWrapLibFunction InSetFunc) :m_GetFunction(InGetFunc), m_SetFunction(InSetFunc) {}
	virtual FCDynamicField* Clone() const { return new FCDynamicWrapLibFunction(*this); }
	virtual int DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
	virtual int DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
    virtual int GetMemSize() const { return sizeof(FCDynamicWrapLibFunction); }
};

struct FCDynamicWrapSystemFunction
{
    const char *FuncName;
    LPLuaLibOpenCallback  m_FunctionPtr;
    FCDynamicWrapSystemFunction() :FuncName(nullptr), m_FunctionPtr(nullptr){}
    FCDynamicWrapSystemFunction(const char *InFuncName, LPLuaLibOpenCallback InFunctionPtr) :FuncName(InFuncName), m_FunctionPtr(InFunctionPtr){}
};

struct FCDynamicWrapLibAttrib : public FCDynamicWrapField
{
	FCDynamicWrapLibAttrib(){}
	FCDynamicWrapLibAttrib(LPWrapLibFunction InGetFunc, LPWrapLibFunction InSetFunc) :FCDynamicWrapField(InGetFunc, InSetFunc) {}
	virtual FCDynamicField* Clone() const { return new FCDynamicWrapLibAttrib(*this); }
	virtual int DoGet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
	virtual int DoSet(lua_State* L, void* ObjRefPtr, void* ClassDescPtr);
    virtual int GetMemSize() const { return sizeof(FCDynamicWrapLibAttrib); }
};

struct FCDelegateInfo
{
    enum 
    {
        CallbackParamMax = 4,
    };
	FCDynamicOverrideFunction*    DynamicFunc;
	FCDynamicProperty*    DynamicProperty;
    const void*   FunctionAddr;
    int           FunctionRef;  // lua 引用的函数
    int           ParamCount;  // lua 回调参数个数
    int           CallbackParams[CallbackParamMax];  // 回调参数，最多4个
    const void*   ParamsValudAddr[CallbackParamMax];
	FCDelegateInfo():DynamicFunc(nullptr), DynamicProperty(nullptr), FunctionAddr(nullptr), FunctionRef(0), ParamCount(0){}
	FCDelegateInfo(const FCDynamicOverrideFunction *InDynamicFunc, const FCDynamicProperty *InDynamicProperty, const void *InFunctionAddr, int InFunctionRef, const int *InCallbackParams, int InCallbackParamCount)
		: DynamicFunc((FCDynamicOverrideFunction*)InDynamicFunc)
		, DynamicProperty((FCDynamicProperty*)InDynamicProperty)
        , FunctionAddr(InFunctionAddr)
		, FunctionRef(InFunctionRef)
		, ParamCount(InCallbackParamCount)
	{
        for(int i = 0; i< CallbackParamMax && i< InCallbackParamCount; ++i)
        {
            CallbackParams[i] = InCallbackParams[i];
            ParamsValudAddr[i] = nullptr;
        }
	}
	bool operator == (const FCDelegateInfo &Other) const
	{
		return DynamicFunc == Other.DynamicFunc
			&& DynamicProperty == Other.DynamicProperty
			&& FunctionAddr == Other.FunctionAddr;
	}
};

struct FCDynamicDelegateList
{
	std::vector<FCDelegateInfo>      Delegates;  // 脚本中绑定的委托方法
	
	FCDynamicDelegateList()
	{
	}
	int FindDelegate(const FCDelegateInfo &Info) const;
		
	// 功能：添加一个委托
	bool  AddScriptDelegate(const FCDelegateInfo &Info);
	bool  DelScriptDelegate(const FCDelegateInfo &Info);
};


typedef  std::unordered_map<const char*, FCDynamicProperty*, FCStringHash, FCStringEqual>   CDynamicName2Property;
typedef  std::unordered_map<int, FCDynamicProperty*>   CDynamicID2Property;
typedef  std::vector<FCDynamicProperty*>   CDynamicPropertyPtrArray;

typedef  std::unordered_map<int, FCDynamicFunction*>   CDynamicFunctionIDMap; // id == > function
typedef  std::unordered_map<const char*, FCDynamicFunction*, FCStringHash, FCStringEqual>   CDynamicFunctionNameMap;  // name ==> function
typedef  std::unordered_map<const char*, FCDynamicField*, FCStringHash, FCStringEqual>   CDynamicFieldNameMap;  // name ==> function

typedef  std::unordered_map<const char*, int, FCStringHash, FCStringEqual>   CDynamicName2Int; // name ==> int
typedef  std::vector<FCDynamicWrapSystemFunction>  CDynamicWrapSystemFunctionList;

const char* GetUEClassName(const char* InName);
int ScriptStruct_Copy_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);
int ScriptStruct_CopyFrom_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject);

// 一个动态类的数据结构
struct FCDynamicClassDesc
{
	UStruct                     *m_Struct;
	UClass                      *m_Class;
    UScriptStruct               *m_ScriptStruct;
	FCDynamicClassDesc          *m_Super;
	EClassCastFlags              m_ClassFlags;  // 用于强制转换的检测
	const char *                 m_SuperName;
	const char *                 m_UEClassName; // 类名，wrap的类名
    int                          m_UserDefinedStructFlag;  // UUserDefinedStruct标记
	CDynamicPropertyPtrArray     m_Property;  // 属性
	CDynamicName2Property        m_Name2Property;  // 所有的属性

	CDynamicFunctionNameMap      m_Functions;   // 所有的函数 name ==> function
	CDynamicFieldNameMap         m_LibFields;   // 扩展属性与方法
    CDynamicFieldNameMap         m_Fileds;      // 属性
    CDynamicWrapSystemFunctionList  m_SystemFunctions;
	LPLuaLibOpenCallback         m_LibOpenCallback;
	
	FCDynamicClassDesc():m_Struct(nullptr), m_Class(nullptr), m_ScriptStruct(nullptr), m_Super(nullptr), m_ClassFlags(CASTCLASS_None), m_SuperName(nullptr), m_UEClassName(nullptr), m_UserDefinedStructFlag(0), m_LibOpenCallback(nullptr)
	{
	}
	~FCDynamicClassDesc();
	FCDynamicClassDesc(const FCDynamicClassDesc &other);
	FCDynamicClassDesc &operator = (const FCDynamicClassDesc &other)
	{
		return CopyDesc(other);
	}

	void Clear();
	FCDynamicClassDesc &CopyDesc(const FCDynamicClassDesc &other);

	void  OnRegisterStruct(UStruct *Struct, void *Context);
    FCDynamicField *RegisterFieldByCString(UStruct* Struct, const char *InFieldName);
    int   GetMemSize() const;

	// 功能：注册一个函数
	FCDynamicFunction*  RegisterUEFunc(const char *pcsFuncName);
	
	// 功能：注册一个类的属性
	FCDynamicProperty*  RegisterProperty(const char *InPropertyName);
	// 功能：注册一个函数
	FCDynamicFunction*  RegisterFunc(const char *pcsFuncName);

	// 添加一个自定义的扩展函数
	FCDynamicField*  RegisterWrapLibFunction(const char* pcsFuncName, LPWrapLibFunction InGetFunc, LPWrapLibFunction InSetFunc);
	// 添加一个自定义的扩展属性
	FCDynamicField*  RegisterWrapLibAttrib(const char* pcsFuncName, LPWrapLibFunction InGetFunc, LPWrapLibFunction InSetFunc);

    // 添加Lua的系统函数
    void AddSystemFunction(const char *InFuncName, LPLuaLibOpenCallback InFuncPtr);

	// table注册时的回调
	void OnLibOpen(lua_State* L);

	// 设置一个回调，就是lua表注册时调用
	void SetLibopenCallback(LPLuaLibOpenCallback callback)
	{
		m_LibOpenCallback = callback;
	}

	FCDynamicFunction  *FindFunctionByName(const char *FuncName)
	{
		CDynamicFunctionNameMap::iterator itFunction = m_Functions.find(FuncName);
		if(itFunction != m_Functions.end())
		{
			return itFunction->second;
        }
        // 没有找到，就动态注册一个
        FCDynamicFunction* Function = RegisterUEFunc(FuncName);
        if (Function)
        {
            return Function;
        }
        // 如果还是没有找到，就是设置一个空的, 避免反复查找
        FuncName = GetConstName(FuncName);
        m_Functions[FuncName] = nullptr;
        return nullptr;
	}
	FCDynamicProperty *FindAttribByName(const char *AttribName)
	{
		CDynamicName2Property::iterator itAttrib = m_Name2Property.find(AttribName);
		if(itAttrib != m_Name2Property.end())
		{
			return itAttrib->second;
		}
        // 没有找到，就动态注册一个
        FCDynamicProperty* Property = RegisterProperty(AttribName);
        if (Property)
        {
            return Property;
        }
        // 如果还是没有找到，就是设置一个空的, 避免反复查找
        AttribName = GetConstName(AttribName);
        m_Name2Property[AttribName] = nullptr;
        return nullptr;
	}
    FCDynamicField  *FindFieldByName(const char *InFieldName)
    {
        CDynamicFieldNameMap::iterator itFiled = m_Fileds.find(InFieldName);
        if(itFiled != m_Fileds.end())
        {
            return itFiled->second;
        }
        // 没有找到，就动态注册一个
		FCDynamicField* Filed = RegisterFieldByCString(m_Struct, InFieldName);
		if (Filed)
		{
			return Filed;
		}
        // 都没有找到的情况下，尝试注册一个空的，以免再次查找
		InFieldName = GetConstName(InFieldName);
		m_Fileds[InFieldName] = nullptr;
        return nullptr;
    }
};

struct FDynamicEnum
{
    struct SDynamicEnumValue
    {
        std::string   Name;
        int           Value;
    };
    std::vector<SDynamicEnumValue>  EnumValue;
    CDynamicName2Int   m_NameToValue;
    void OnRegisterEnum(UEnum* InEnum);
    int FindEnumValue(const char *InName) const
    {
        if(!InName)
            return 0;
        CDynamicName2Int::const_iterator itValue = m_NameToValue.find(InName);
        if(itValue != m_NameToValue.end())
            return itValue->second;
        return 0;
    }
};

typedef std::unordered_map<const char *, FCDynamicClassDesc*, FCStringHash, FCStringEqual>   CDynamicClassNameMap;
typedef std::unordered_map<std::string, FDynamicEnum*>   CDynamicEnumNameMap;
typedef std::unordered_map<int, FCDynamicClassDesc*>   CDynamicClassIDMap;
typedef std::unordered_map<UStruct*, FCDynamicClassDesc*>   CDynamicUStructMap;
typedef std::unordered_map<FProperty*, FCDynamicClassDesc*>   CDynamicPropertyMap;
typedef std::unordered_map<lua_State*, int32>   ThreadToRefMap;
typedef std::unordered_map<int32, lua_State*>   RefToThreadMap;

struct FCScriptContext
{
	bool                  m_bInit;
	lua_State            *m_LuaState;
    UObject              *m_Ticker;
    UObject              *m_DelegateObject;

	CDynamicClassNameMap  m_ClassNameMap;   // name == > class ptr
	CDynamicClassNameMap  m_ClassFinder;   // name == > class ptr
    CDynamicEnumNameMap   m_EnumNameMap;    // name == > class ptr
	CDynamicUStructMap    m_StructMap;      // UStruct* ==> class ptr
    CDynamicPropertyMap   m_PropeytyMap;    // FPropery ==> class ptr

    ThreadToRefMap        m_ThreadToRef;
    RefToThreadMap        m_RefToThread;

    FCObjectReferencer    *m_ManualObjectReference;

    FCDynamicWrapLibFunction  *m_CopyWrapFunc;
    FCDynamicWrapLibFunction  *m_CopyFromWrapFunc;

	FCScriptContext():m_bInit(false), m_LuaState(nullptr), m_Ticker(nullptr), m_DelegateObject(nullptr), m_ManualObjectReference(nullptr)
        , m_CopyWrapFunc(nullptr), m_CopyFromWrapFunc(nullptr)
	{
	}

    int  QueryLuaRef(lua_State* L);
    void ResumeThread(int ThreadRef);
	
    FCDynamicClassDesc*  RegisterCppClass(const char* UEClassName);
	FCDynamicClassDesc*  RegisterUClass(const char *UEClassName);
	FCDynamicClassDesc*  RegisterUStruct(UStruct *Struct);
    FCDynamicClassDesc*  RegisterByProperty(FProperty *Property);
    FDynamicEnum*        RegisterEnum(const char *InEnumName);
    int GetMemSize() const;
    int GetClassMemSize(const char *InClassName) const;
    void Init();
	void Clear();
};

struct FCContextManager
{
	FCScriptContext   ClientDSContext;  // Client + DS, 不再区分DS
	static  FCContextManager  *ConextMgrInsPtr;
	FCContextManager();
	~FCContextManager();
    void Init();
	void Clear();
};

inline FCContextManager *GetContextManger()
{
	return FCContextManager::ConextMgrInsPtr;
}

inline FCScriptContext  *GetClientScriptContext()
{
	return &(FCContextManager::ConextMgrInsPtr->ClientDSContext);
}

inline FCScriptContext  *GetScriptContext()
{
	return &(FCContextManager::ConextMgrInsPtr->ClientDSContext);
}
