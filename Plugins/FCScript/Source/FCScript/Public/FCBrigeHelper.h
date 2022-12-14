#pragma once

#include "FCBrigeBase.h"
#include "FCStringCore.h"

typedef  int (*exportlib_class_call_back)(lua_State* L, char* ThisAddr, void* ClassDesc);
typedef  int (*exportlib_custom_call_back)(lua_State* L);

struct FCExportedItem
{
	const char* Name;
	const char* ClassName;
	FCExportedItem *Next;
	FCExportedItem():Name(nullptr), ClassName(nullptr), Next(nullptr){}
	FCExportedItem(const char *InName, const char *InClassName):Name(InName), ClassName(InClassName), Next(nullptr){}
	virtual ~FCExportedItem(){}
    virtual bool IsFunction() const { return false; }
    virtual bool IsLibFunction() const{ return false; }
	virtual bool IsLibAttrib() const { return false; }
    virtual bool IsCanSet() const{ return false; }  // 是不是可以set, 可写属性
    virtual int Read(lua_State* L, char *ThisAddr, void *ClassDesc) const { return 0; }
    virtual int Write(lua_State* L, char* ThisAddr, void* ClassDesc) const { return 0; }
    virtual int Invoke(lua_State* L, char* ThisAddr, void* ClassDesc) const { return 0; }
};

struct FCExportInvalidItem : public FCExportedItem
{
    std::string    SelfName;
    FCExportInvalidItem(){}
    FCExportInvalidItem(const char *InName, const char *InClassName):FCExportedItem(InName, InClassName), SelfName(InName)
    {
        Name = SelfName.c_str();
    }
    virtual int Read(lua_State* L, char* ThisAddr, void* ClassDesc) const
    {
        lua_pushnil(L);
        return 1;
    }
};

struct FCExportProperty : public FCExportedItem
{
	int          Offset;        // 偏移
	int          Aim;           // 数组长度

	FCExportProperty() :Offset(0), Aim(0){}
	FCExportProperty(const char* InPropertyName, int InOffset, int InAim, const char *InClassName) :FCExportedItem(InPropertyName, InClassName), Offset(InOffset), Aim(InAim){}
};

struct FCExportFunction : public FCExportedItem
{
    exportlib_class_call_back  FuncPtr;
	FCExportFunction():FuncPtr(nullptr){}
	FCExportFunction(const char *InFuncName, const char *InClassName):FCExportedItem(InFuncName, InClassName), FuncPtr(nullptr){}

	virtual int Invoke(lua_State* L, char* ThisAddr, void* ClassDesc) const
	{
		if(FuncPtr)
		{
			return FuncPtr(L, ThisAddr, ClassDesc);
		}
        return 0;
    }
    virtual bool IsFunction() const 
	{
		return true; 
	}
};

struct FCExportLibFunction : public FCExportedItem
{
    exportlib_custom_call_back Func;
	FCExportLibFunction():Func(nullptr){}
	FCExportLibFunction(const char* InFuncName, const char *InClassName, exportlib_custom_call_back InFunc) :FCExportedItem(InFuncName, InClassName), Func(InFunc) {}
    virtual bool IsFunction() const { return true; }
    virtual bool IsLibFunction() const { return true; }
};

struct FCExportLibAttrib : public FCExportLibFunction
{
    exportlib_custom_call_back  SetFunc;
	FCExportLibAttrib() :SetFunc(nullptr) {}
	FCExportLibAttrib(const char* InFuncName, const char* InClassName, exportlib_custom_call_back InGetFunc, exportlib_custom_call_back InSetFunc) :FCExportLibFunction(InFuncName, InClassName, InGetFunc), SetFunc(InSetFunc) {}
    virtual bool IsLibFunction() const { return true; }
	virtual bool IsLibAttrib() const { return true; }
    virtual bool IsCanSet() const { return true; }  // 是不是可以set, 可写属性
};

struct LuaRegFunc
{
    const char *Name;
    exportlib_custom_call_back Func;
};

struct LuaRegAttrib
{
    const char* Name;
    exportlib_custom_call_back GetFunc;
    exportlib_custom_call_back SetFunc;
};

typedef  stdext::hash_map<const char*, FCExportedItem*>   CExportedItemMap;

struct FCSCRIPT_API FCExportedClass
{
	static FCExportedClass  *s_pExportedIns;
	FCExportedClass(const char *InName):ClassName(InName)
		, NextClass(s_pExportedIns)
		, Propertys(nullptr)
		, Functions(nullptr)
        , InvalidItems(nullptr)
		, bCustomRegistered(false)
	{
		s_pExportedIns = this;
	}

	virtual ~FCExportedClass() {}

	const char* GetName() const
	{
		return ClassName;
	}
	static FCExportedClass* FindExportedClass(const char* InClassName);
	static void RegisterAll(lua_State* L);
	static void UnRegisterAll(lua_State* L);
    static int RegisterLibClass(lua_State* L, const char* InClassName, const LuaRegFunc* Funcs);
    int RegisterLibClass(lua_State* L, const char *InClassName, const LuaRegFunc* Funcs, const LuaRegAttrib* Attribs, const LuaRegFunc* TableFuncs);
	static void* GetThisPtr(lua_State* L, const char* InClassName);
	static void ReleaseList(FCExportedItem *InList);
    static const FCExportedItem* FindChildFromList(const FCExportedItem* InListPtr, const char* InName);
    static int obj_Index(lua_State* L);
    static int obj_NewIndex(lua_State* L);
    static int obj_Identical(lua_State* L);
    static int obj_equal(lua_State* L);
    static int obj_release(lua_State* L);
    static int obj_Delete(lua_State* L);
    static int GetAttrib_wrap(lua_State* L);
    static int SetAttrib_wrap(lua_State* L);
    static int Function_wrap(lua_State* L);

	virtual void InitFunctionList() {}

	void Register(lua_State* L);
	void UnRegister(lua_State* L);

    int  DoLibIndex(lua_State* L);
    int  DoLibNewIndex(lua_State* L);

	void AddClassAttrib(FCExportProperty* InProperty)
	{
		InProperty->Next = Propertys;
		Propertys = InProperty;
	}

    // 添加一个wrap方法
	void AddLibFunction(const char* InName, exportlib_custom_call_back InFuncCallback)
	{
		if(InName && InFuncCallback)
		{
			FCExportLibFunction *Func = new FCExportLibFunction(InName, ClassName, InFuncCallback);
            AddClassFunction(Func);
		}
	}
    // 添加一个属性方法(get + set)
    void AddLibAttrib(const char *InName, exportlib_custom_call_back InGetFunc, exportlib_custom_call_back InSetFunc)
    {
		FCExportLibAttrib*Func = new FCExportLibAttrib(InName, ClassName, InGetFunc, InSetFunc);
        AddClassFunction(Func);
    }
	void AddFunctionLibs(const LuaRegFunc *lib, int InFuncCount)
	{
		for(int i = 0; i<InFuncCount; ++i)
		{
			AddLibFunction(lib[i].Name, lib[i].Func);
		}
	}
	void AddClassFunction(FCExportedItem*Func)
	{
		Func->Next = Functions;
		Functions = Func;
	}
	const FCExportProperty* FindClassProperty(const char* InPropertyName) const
	{
		return (const FCExportProperty*)FindChildFromList(Propertys, InPropertyName);
	}
	const FCExportedItem* FindClassFunction(const char *InFuncName) const
	{
		return FindChildFromList(Functions, InFuncName);
	}

	const FCExportedItem  *GetChildItem(const char *InChildName)
	{
        CExportedItemMap::local_iterator itChild = ChildItmes.find(InChildName);
        if(itChild != ChildItmes.end())
        {
            return itChild->second;
        }
        const FCExportProperty *Property = FindClassProperty(InChildName);
        if(Property)
        {
            ChildItmes[Property->Name] = (FCExportProperty *)Property;
            return Property;
        }
        const FCExportedItem*Function = FindClassFunction(InChildName);
        if(Function)
        {
            ChildItmes[Function->Name] = (FCExportFunction*)Function;
            return Function;
        }
        FCExportedItem *InvalidItem = AddInvalidItem(InChildName);
        ChildItmes[InvalidItem->Name] = InvalidItem;
		return InvalidItem;
	}
	void SetCustomRegister()
	{
		bCustomRegistered = true;
	}

    FCExportedItem *AddInvalidItem(const char *InChildName);

	const char *ClassName;
	FCExportedClass* NextClass;
	FCExportProperty* Propertys;   // 属性
    FCExportedItem*   Functions;   // 成员函数+全局函数

	//-----------下面的是动态注册时自动维护的变量，请不要在外面修改
    FCExportedItem    *InvalidItems;
	bool              bCustomRegistered;
    CExportedItemMap   ChildItmes;  // 子节点
	//----------------
};

#define GetClassPropertyOffset(PropertyName)  (int)((size_t)(&((ClassType*)0)->PropertyName))

// 成员属性
template <typename ClassType, typename T>
struct TFCExportProperty : public FCExportProperty
{
	TFCExportProperty(const char* InName, int InOffset, int InAim, const char *InClassName) :FCExportProperty(InName, InOffset, InAim, InClassName)
	{
	}
	virtual int Read(lua_State* L, char* ThisAddr, void* ClassDesc) const
	{
		ClassType* ThisObj = (ClassType*)FCExportedClass::GetThisPtr(L, ClassName);
		if(ThisObj)
		{
			T *Property = (T *)(ThisAddr + Offset);
			FCScript::SetArgValue(L, *Property);
		}
        else
        {
            lua_pushnil(L);
        }
        return 1;
	}
    // (t, key, value)
    // t[key] = value
	virtual int Write(lua_State* L, char* ThisAddr, void* ClassDesc) const
	{
		ClassType* ThisObj = (ClassType*)FCExportedClass::GetThisPtr(L, ClassName);
		if (ThisObj)
		{
			T* Property = (T*)(ThisAddr + Offset);
			FCScript::GetArgValue(L, 3, *Property);
		}
        return 0;
	}
};

// 成员函数
template <typename ClassType, typename RetType, typename... ArgType>
struct TFCExportedMemberFunction : public FCExportFunction
{
	typedef  RetType(ClassType::* LPClassMemberFuncPtrNoraml)(ArgType...);
	typedef  RetType(ClassType::* LPClassMemberFuncPtrConst)(ArgType...) const;

	TFCExportedMemberFunction(const char* InName, RetType(ClassType::* InFunc)(ArgType...), const char* InClassName) :FCExportFunction(InName, InClassName), FuncNormal(InFunc)
	{
	}
	TFCExportedMemberFunction(const char* InName, RetType(ClassType::* InFunc)(ArgType...) const, const char* InClassName) :FCExportFunction(InName, InClassName), FuncNormal((LPClassMemberFuncPtrNoraml)InFunc)
	{
	}

	virtual int Invoke(lua_State* L, char* ThisAddr, void* ClassDesc) const
	{
		ClassType *ThisObj = (ClassType *)FCExportedClass::GetThisPtr(L, ClassName);
		if(ThisObj)
        {
            return FCInvokeClassFunction<RetType, ClassType, ArgType...>(L, ThisObj, FuncNormal);
		}
        else
        {
            return 0;
        }
	}

	LPClassMemberFuncPtrNoraml FuncNormal;
};


// 全局函数
template <typename RetType, typename... ArgType>
struct TFCExportedStaticMemberFunction : public FCExportFunction
{
	typedef  RetType(* LPClassMemberFuncPtrNoraml)(ArgType...);
	TFCExportedStaticMemberFunction(const char* InName, RetType(*InFunc)(ArgType...), const char * InClassName):FCExportFunction(InName, InClassName), FuncNormal(InFunc)
	{
	}
	virtual int Invoke(lua_State* L, char* ThisAddr, void* ClassDesc) const
	{
		return FCInvokeStaticFunction(L, FuncNormal);
	}
	LPClassMemberFuncPtrNoraml FuncNormal;
};

template <typename ClassType, typename... CtorArgType>
struct TFCExportedClass : public FCExportedClass
{
	TFCExportedClass(const char* InName):FCExportedClass(InName)
	{
	}

	bool AddBitFieldBoolProperty(const char* InName, uint8* Buffer)
	{
		return false;
	}

	template <typename T> void AddMemberProperty(const char* InName, int InOffset, T ClassType::* Property)
	{
		AddClassAttrib(new TFCExportProperty<ClassType, T>(InName, InOffset, 1, ClassName));
	}
	template <typename T, int N> void AddMemberProperty(const char* InName, int InOffset, T(ClassType::* Property)[N])
	{
		AddClassAttrib(new TFCExportProperty<ClassType, T>(InName, InOffset, N, ClassName));
	}

	template <typename RetType, typename... ArgType> void AddMemberFunction(const char* InName, RetType(ClassType::* InFunc)(ArgType...))
	{
		AddClassFunction(new TFCExportedMemberFunction<ClassType, RetType, ArgType...>(InName, InFunc, ClassName));
	}
	template <typename RetType, typename... ArgType> void AddMemberFunction(const char* InName, RetType(ClassType::* InFunc)(ArgType...) const)
	{
		AddClassFunction(new TFCExportedMemberFunction<ClassType, RetType, ArgType...>(InName, InFunc, ClassName));
	}
	template <typename RetType, typename... ArgType> void AddStaticFunction(const char* InName, RetType(*InFunc)(ArgType...))
	{
		AddClassFunction(new TFCExportedStaticMemberFunction<RetType, ArgType...>(InName, InFunc, ClassName));
	}
};

#define BEGIN_EXPORT_CLASS(Type) \
    struct FExported##Type##Helper : public TFCExportedClass<Type> \
    { \
        typedef Type ClassType; \
        FExported##Type##Helper() : TFCExportedClass(#Type) \
		{ \
		} \
        void InitFunctionList() \
        { 

#define ADD_PROPERTY(Property) \
            this->AddMemberProperty(#Property, GetClassPropertyOffset(Property), &ClassType::Property);

#define ADD_STATIC_FUNCTION_EX(FuncName, RetType, Func) \
			this->AddStaticFunction(FuncName, &ClassType::Func);

#define ADD_STATIC_FUNCTION(Func) \
			this->AddStaticFunction(#Func, &ClassType::Func);

#define ADD_FUNCTION(Func) \
			this->AddMemberFunction(#Func, &ClassType::Func);

#define ADD_LIB(libs) \
			this->AddFunctionLibs(libs, sizeof(libs)/sizeof(LuaRegFunc));

#define END_EXPORT_CLASS()  \
        } \
	};

#define IMPLEMENT_EXPORTED_CLASS(Type) \
FExported##Type##Helper SExport##Type##HelperIns;
