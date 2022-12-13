#include "FCTemplateType.h"


#if ENGINE_MINOR_VERSION < 25 
template<class _Ty>
_Ty * NewUEProperty(UScriptStruct* ScriptStruct)
{
	_Ty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) _Ty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash);
	return Property;
}
#else 
template<class _Ty>
_Ty* NewUEProperty(UScriptStruct* ScriptStruct)
{
	_Ty* Property = new _Ty(ScriptStruct, NAME_None, RF_Transient, 0, CPF_HasGetValueTypeHash);
	return Property;
}
#endif 

FProperty  *NewUEBoolProperty(UScriptStruct* ScriptStruct)
{
#if ENGINE_MINOR_VERSION < 25
	// see overloaded operator new that defined in DECLARE_CLASS(...)
	UBoolProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UBoolProperty(FObjectInitializer(), EC_CppProperty, 0, (EPropertyFlags)0, 0xFF, 1, true);
#else
	FBoolProperty* Property = new FBoolProperty(ScriptStruct, NAME_None, RF_Transient, 0, (EPropertyFlags)0, 0xFF, 1, true);
#endif
	return Property;
}

FProperty* NewUEStructProperty(UScriptStruct* Struct, UScriptStruct* ScriptStruct)
{
#if ENGINE_MINOR_VERSION < 25
	// see overloaded operator new that defined in DECLARE_CLASS(...)
	FStructProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UObjectProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash, Struct);
#else
	FStructProperty* Property = new FStructProperty(ScriptStruct, NAME_None, RF_Transient, 0, CPF_HasGetValueTypeHash, Struct);
#endif
	return Property;
}

FProperty  *NewUEClassProperty(UClass *Class, UScriptStruct* ScriptStruct)
{
#if ENGINE_MINOR_VERSION < 25
	// see overloaded operator new that defined in DECLARE_CLASS(...)
	UObjectProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UObjectProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash, Class);
#else
	FObjectProperty* Property = new FObjectProperty(ScriptStruct, NAME_None, RF_Transient, 0, CPF_HasGetValueTypeHash, Class);
#endif
	return Property;
}

FArrayProperty  *NewUEArrayProperty(UScriptStruct* ScriptStruct)
{
#if ENGINE_MINOR_VERSION < 25
	FArrayProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) FArrayProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_None);
#else
	FArrayProperty* Property = new FArrayProperty(ScriptStruct, NAME_None, RF_Transient);
#endif
	return Property;
}

FMapProperty  *NewUEMapProperty(UScriptStruct* ScriptStruct)
{	
#if ENGINE_MINOR_VERSION < 25
	FMapProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) FMapProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_None);
#else
	FMapProperty* Property = new FMapProperty(ScriptStruct, NAME_None, RF_Transient);
#endif
	return Property;
}

FSetProperty* NewUESetProperty(UScriptStruct* ScriptStruct)
{
#if ENGINE_MINOR_VERSION < 25
	FSetProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) FMapProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_None);
#else
	FSetProperty* Property = new FSetProperty(ScriptStruct, NAME_None, RF_Transient);
#endif
    return Property;
}

UScriptStruct   *GScriptStruct = nullptr;

UScriptStruct   *GetGlbScriptStruct()
{
	if(!GScriptStruct)
	{
		GScriptStruct = FindObject<UScriptStruct>(ANY_PACKAGE, TEXT("LuaPropertyCollector"));
	}
	return GScriptStruct;
}

const char* GetInnerTypeName(FCInnerBaseType InBaseType)
{
    switch (InBaseType)
    {
    case FC_INNER_TYPE_bool:
        return "bool";
    case FC_INNER_TYPE_INT8:
        return "int8";
    case FC_INNER_TYPE_UINT16:
        return "uint16";
    case FC_INNER_TYPE_INT16:
        return "int16";
    case FC_INNER_TYPE_INT:
        return "int";
    case FC_INNER_TYPE_UINT:
        return "uint";
    case FC_INNER_TYPE_FLOAT:
        return "float";
    case FC_INNER_TYPE_DOUBLE:
        return "double";
    case FC_INNER_TYPE_INT64:
        return "int64";
    case FC_INNER_TYPE_UINT64:
        return "uint64";
    case FC_INNER_TYPE_FNAME:
        return "FName";
    case FC_INNER_TYPE_FSTRING:
        return "FString";
	case FC_INNER_TYPE_FTEXT:
		return "FText";
    default:
        break;
    }
    return nullptr;
}

FProperty  *CreateBaseProperty(FCInnerBaseType InBaseType)
{
	UScriptStruct* ScriptStruct = GetGlbScriptStruct();
	switch(InBaseType)
	{
		case FC_INNER_TYPE_bool:
			return NewUEBoolProperty(ScriptStruct);
		case FC_INNER_TYPE_INT8:
			return NewUEProperty<FByteProperty>(ScriptStruct);
		case FC_INNER_TYPE_UINT16:
			return NewUEProperty<FUInt16Property>(ScriptStruct);
		case FC_INNER_TYPE_INT16:
			return NewUEProperty<FInt16Property>(ScriptStruct);
		case FC_INNER_TYPE_INT:
			return NewUEProperty<FIntProperty>(ScriptStruct);
		case FC_INNER_TYPE_UINT:
			return NewUEProperty<FUInt32Property>(ScriptStruct);
		case FC_INNER_TYPE_FLOAT:
			return NewUEProperty<FFloatProperty>(ScriptStruct);
		case FC_INNER_TYPE_DOUBLE:
			return NewUEProperty<FDoubleProperty>(ScriptStruct);
		case FC_INNER_TYPE_INT64:
			return NewUEProperty<FInt64Property>(ScriptStruct);
		case FC_INNER_TYPE_UINT64:
			return NewUEProperty<FUInt64Property>(ScriptStruct);
		case FC_INNER_TYPE_FNAME:
			return NewUEProperty<FNameProperty>(ScriptStruct);
		case FC_INNER_TYPE_FSTRING:
			return NewUEProperty<FStrProperty>(ScriptStruct);
		case FC_INNER_TYPE_FTEXT:
			return NewUEProperty<FTextProperty>(ScriptStruct);
		default:
			break;
	}
	return nullptr;
}

typedef stdext::hash_map<const char *, FCInnerBaseType> CInnerTypeMap;
typedef stdext::hash_map<const char *, FProperty*> CTemplatePropertyNameMap;
typedef stdext::hash_map<UStruct*, FCDynamicProperty*> CStructDynamicPropertyMap;
typedef stdext::hash_map<FProperty*, FCDynamicProperty*> CPropertyDynamicPropertyMap;
typedef stdext::hash_map<const char*, FCDynamicProperty*> CCppDynamicPropertyMap;
typedef stdext::hash_map<const char*, char*> CCppName2NameMap;
CInnerTypeMap            GInnerTypeMap;
CTemplatePropertyNameMap GClassPropertyNameMap;
CStructDynamicPropertyMap GStructDynamicPropertyMap;
CPropertyDynamicPropertyMap GPropertyDynamicPropertyMap;
CCppDynamicPropertyMap      GCppDynamicPropertyMap;
CCppName2NameMap            GCppName2NameMap;

FCInnerBaseType GetInnerType(const char *InClassName)
{
    if(GInnerTypeMap.empty())
    {
        GInnerTypeMap["bool"] = FC_INNER_TYPE_bool;
        GInnerTypeMap["byte"] = FC_INNER_TYPE_INT8;
        GInnerTypeMap["int8"] = FC_INNER_TYPE_INT8;
        GInnerTypeMap["int16"] = FC_INNER_TYPE_INT16;
        GInnerTypeMap["uint16"] = FC_INNER_TYPE_UINT16;
        GInnerTypeMap["int"] = FC_INNER_TYPE_INT;
        GInnerTypeMap["uint"] = FC_INNER_TYPE_UINT;
        GInnerTypeMap["float"] = FC_INNER_TYPE_FLOAT;
        GInnerTypeMap["double"] = FC_INNER_TYPE_DOUBLE;
        GInnerTypeMap["int64"] = FC_INNER_TYPE_INT64;
        GInnerTypeMap["uint64"] = FC_INNER_TYPE_UINT64;
        GInnerTypeMap["FName"] = FC_INNER_TYPE_FNAME;
        GInnerTypeMap["FString"] = FC_INNER_TYPE_FSTRING;
		GInnerTypeMap["FText"] = FC_INNER_TYPE_FTEXT;
    }
    CInnerTypeMap::const_iterator itType = GInnerTypeMap.find(InClassName);
    if(itType != GInnerTypeMap.end())
    {
        return itType->second;
    }
    return FC_INNER_TYPE_Unknow;
}

const char* GetConstName(const char* InName)
{
	if (!InName)
		return "";
	CCppName2NameMap::iterator itName = GCppName2NameMap.find(InName);
	if (itName != GCppName2NameMap.end())
	{
		return itName->second;
	}
	int  Len = strlen(InName);
	char* buffer = new char[Len + 1];
	memcpy(buffer, InName, Len);
	buffer[Len] = 0;
	GCppName2NameMap[buffer] = buffer;
	return buffer;
}

FProperty  *CreateClassProperty(const char *InClassName)
{
	CTemplatePropertyNameMap::iterator itProperty = GClassPropertyNameMap.find(InClassName);
	if(itProperty != GClassPropertyNameMap.end())
	{
		return itProperty->second;
	}
	const FCDynamicClassDesc *DynamicClass = GetScriptContext()->RegisterUClass(InClassName);
	if(!DynamicClass)
	{
        FCInnerBaseType InnerType = GetInnerType(InClassName);
        if(InnerType != FC_INNER_TYPE_Unknow)
        {
            InClassName = GetInnerTypeName(InnerType);
            FProperty* Property = CreateBaseProperty(InnerType);
            GClassPropertyNameMap[InClassName] = Property;
            return Property;
        }
        // not surport type
		return nullptr;
	}
	if(!DynamicClass->m_Class)
	{
		if (DynamicClass->m_Struct)
		{
			// 注明一下，这里的Struct一定是UScriptStruct
			FProperty* Property = NewUEStructProperty((UScriptStruct *)DynamicClass->m_Struct, GetGlbScriptStruct());
			InClassName = DynamicClass->m_UEClassName.c_str();
			GClassPropertyNameMap[InClassName] = Property;
			return Property;
		}
		return nullptr;
	}
	FProperty *Property = NewUEClassProperty(DynamicClass->m_Class, GetGlbScriptStruct());
	InClassName = DynamicClass->m_UEClassName.c_str();
	GClassPropertyNameMap[InClassName] = Property;
	return Property;
}

FCDynamicProperty* GetCppDynamicProperty(const char* InClassName)
{
	CCppDynamicPropertyMap::iterator itProperty = GCppDynamicPropertyMap.find(InClassName);
	if(itProperty != GCppDynamicPropertyMap.end())
	{
		return itProperty->second;
    }
	FProperty* Property = CreateClassProperty(InClassName);
	if(Property)
    {
        FCDynamicProperty* DynamicPropery = new FCDynamicProperty();
		DynamicPropery->InitProperty(Property);
		GCppDynamicPropertyMap[InClassName] = DynamicPropery;
		return DynamicPropery;
	}
	else
	{
		GCppDynamicPropertyMap[InClassName] = nullptr;
		return nullptr;
	}
}

FCDynamicProperty* GetStructDynamicProperty(UStruct* Struct)
{
	CStructDynamicPropertyMap::iterator itProperty = GStructDynamicPropertyMap.find(Struct);
	if (itProperty != GStructDynamicPropertyMap.end())
	{
		return itProperty->second;
	}
	FCDynamicProperty* DynamicPropery = new FCDynamicProperty();
	FProperty* Property = NewUEStructProperty((UScriptStruct*)Struct, GetGlbScriptStruct());
	DynamicPropery->InitProperty(Property);
	DynamicPropery->Name = TCHAR_TO_UTF8(*(Struct->GetName()));
	GStructDynamicPropertyMap[Struct] = DynamicPropery;
	return DynamicPropery;
}

FCDynamicProperty* GetDynamicPropertyByUEProperty(FProperty* InProperty)
{
	CPropertyDynamicPropertyMap::iterator itProperty = GPropertyDynamicPropertyMap.find(InProperty);
	if (itProperty != GPropertyDynamicPropertyMap.end())
	{
		return itProperty->second;
	}
	FCDynamicProperty* DynamicPropery = new FCDynamicProperty();
	DynamicPropery->InitProperty(InProperty);
	if(FCPropertyType::FCPROPERTY_StructProperty == DynamicPropery->Type)
	{
		FStructProperty* StructProperty = (FStructProperty*)InProperty;
		UStruct* Struct = StructProperty->Struct;
		DynamicPropery->Name = TCHAR_TO_UTF8(*(Struct->GetName()));
	}
	GPropertyDynamicPropertyMap[InProperty] = DynamicPropery;
	return DynamicPropery;
}

FProperty *QueryTempalteProperty(lua_State* L, const char* InClassName)
{
	FProperty *Property = CreateClassProperty(InClassName);
	return Property;
}

FArrayProperty* CreateTArrayProperty(lua_State* L, const char* InPropertyType)
{
	FProperty *Property = QueryTempalteProperty(L, InPropertyType);
	if(!Property)
	{
		return nullptr;
	}
	FArrayProperty *ArrayProperty = NewUEArrayProperty(GetGlbScriptStruct());
	ArrayProperty->Inner = Property;	
	return ArrayProperty;
}

struct FCTArrayDynamicProperty : public FCDynamicProperty
{
	FArrayProperty  *ArrayProperty;
	FCTArrayDynamicProperty(FArrayProperty *InArrayProperty):ArrayProperty(InArrayProperty)
	{
	}
	~FCTArrayDynamicProperty()
	{
		if(ArrayProperty)
		{
			delete ArrayProperty;
		}
	}
};

typedef stdext::hash_map<const char *, FCDynamicProperty*> CTempalteDynamicPropertyMap;
typedef stdext::hash_map<FCDoubleKey, FCDynamicProperty*> CMapTempalteDynamicPropertyMap;
CTempalteDynamicPropertyMap   GTempalteDynamicPropertyMap;
CMapTempalteDynamicPropertyMap GMapTemplateDynamicPropertyMap;

const char *GetPropertyType(lua_State* L, int Idx)
{
    int32 Type = lua_type(L, Idx);
    switch (Type)
    {
    case LUA_TBOOLEAN:
        return "bool";
    case LUA_TNUMBER:
        return lua_isinteger(L, Idx) > 0 ? "int" : "float";
    case LUA_TSTRING:
        {
            const char *Name = lua_tostring(L, Idx);
            if(Name == NULL || Name[0] == 0)
                return "FString";
            return Name;
        }
    case LUA_TTABLE:
        {
            lua_pushstring(L, "__name");
            Type = lua_rawget(L, Idx);
            if (Type == LUA_TSTRING)
            {
                const char *Name = lua_tostring(L, -1);
                lua_pop(L, 1);
                return Name;
            }
            lua_pop(L, 1);
        case LUA_TUSERDATA:
            break;
        }
        break;
    default:
        break;
    }
    return "FString";
}

FCDynamicProperty* GetTArrayDynamicProperty(lua_State* L)
{
    const char *PropertyName = GetPropertyType(L, 2);
	CTempalteDynamicPropertyMap::iterator itProperty = GTempalteDynamicPropertyMap.find(PropertyName);
	if(itProperty != GTempalteDynamicPropertyMap.end())
	{
		return itProperty->second;
	}
	PropertyName = GetConstName(PropertyName);
	FArrayProperty* ArrayProperty = CreateTArrayProperty(L, PropertyName);
	if(!ArrayProperty)
	{
		GTempalteDynamicPropertyMap[PropertyName] = nullptr;
		return nullptr;
	}
	FCTArrayDynamicProperty  *DynamicProperty = new FCTArrayDynamicProperty(ArrayProperty);
	DynamicProperty->InitProperty(ArrayProperty);
	GTempalteDynamicPropertyMap[PropertyName] = DynamicProperty;
	return DynamicProperty;
}

//---------------------------------------------------------------------

FMapProperty* CreateTMapProperty(lua_State* L, const char *KeyType, const char *ValueType)
{
	FProperty *KeyProperty = QueryTempalteProperty(L, KeyType);
	FProperty *ValueProperty = QueryTempalteProperty(L, ValueType);
	if(!KeyProperty || !ValueProperty)
	{
		return nullptr;
	}
	FMapProperty  *MapProperty = NewUEMapProperty(GetGlbScriptStruct());
	MapProperty->KeyProp = KeyProperty;
	MapProperty->ValueProp = ValueProperty;

	int ValueSize = ValueProperty->ElementSize * ValueProperty->ArrayDim;
	int AlignKeySize = (KeyProperty->ElementSize + 7)/8*8;
	int AlignValueSize = (ValueSize + 7)/8*8;

	MapProperty->MapLayout = FScriptMap::GetScriptLayout(KeyProperty->ElementSize, AlignKeySize, ValueSize, AlignValueSize);

	return MapProperty;
}

struct FCTMapDynamicProperty : public FCDynamicProperty
{
	FMapProperty  *MapProperty;
	FCTMapDynamicProperty(FMapProperty *InMapProperty):MapProperty(InMapProperty)
	{
	}
	~FCTMapDynamicProperty()
	{
		if(MapProperty)
		{
			//delete MapProperty; // 这个不能释放，UE会自动释放，不然就会Crash
		}
	}
};

FCDynamicProperty *GetTMapDynamicProperty(lua_State* L)
{
	const char* KeyTypeName = GetPropertyType(L, 2);
	const char* ValueTypeName = GetPropertyType(L, 3);
	FCDoubleKey  MapKey(KeyTypeName, ValueTypeName);
	CMapTempalteDynamicPropertyMap::iterator itProperty = GMapTemplateDynamicPropertyMap.find(MapKey);
	if(itProperty != GMapTemplateDynamicPropertyMap.end())
	{
		return itProperty->second;
	}
	KeyTypeName = GetConstName(KeyTypeName);
	ValueTypeName = GetConstName(ValueTypeName);
	MapKey = FCDoubleKey(KeyTypeName, ValueTypeName);

	FMapProperty  *MapProperty = CreateTMapProperty(L, KeyTypeName, ValueTypeName);
	if(!MapProperty)
	{
		GMapTemplateDynamicPropertyMap[MapKey] = nullptr;
		return nullptr;
	}
	FCTMapDynamicProperty  *DynamicProperty = new FCTMapDynamicProperty(MapProperty);
	DynamicProperty->InitProperty(MapProperty);
	GMapTemplateDynamicPropertyMap[MapKey] = DynamicProperty;

	return DynamicProperty;
}

struct FCTSetDynamicProperty : public FCDynamicProperty
{
    FSetProperty* SetProperty;
	FCTSetDynamicProperty(FSetProperty* InSetProperty) :SetProperty(InSetProperty)
    {		
    }
    ~FCTSetDynamicProperty()
    {
        if (SetProperty)
        {
            delete SetProperty;
        }
    }
};

//-----------------------------------------------
FSetProperty* CreateTSetProperty(lua_State* L, const char* ClassName)
{
    FProperty* ElementProp = QueryTempalteProperty(L, ClassName);
    if (!ElementProp)
    {
        return nullptr;
    }
    FSetProperty* SetProperty = NewUESetProperty(GetGlbScriptStruct());
	SetProperty->ElementProp = ElementProp;

    int ValueSize = ElementProp->ElementSize * ElementProp->ArrayDim;
    int AlignValueSize = (ValueSize + 7) / 8 * 8;

	SetProperty->SetLayout = FScriptSet::GetScriptLayout(ValueSize, AlignValueSize);

    return SetProperty;
}

FCDynamicProperty* GetTSetDynamicProperty(lua_State* L)
{
    // 说明，由于TMap与TArray的参数不一样，所以不会存在相同的TemplateID, 这里共用一个模板列表吧
	const char* KeyTypeName = GetPropertyType(L, 2);
    CTempalteDynamicPropertyMap::iterator itProperty = GTempalteDynamicPropertyMap.find(KeyTypeName);
    if (itProperty != GTempalteDynamicPropertyMap.end())
    {
        return itProperty->second;
    }
	KeyTypeName = GetConstName(KeyTypeName);
    FSetProperty* SetProperty = CreateTSetProperty(L, KeyTypeName);
    if (!SetProperty)
    {
        GTempalteDynamicPropertyMap[KeyTypeName] = nullptr;
        return nullptr;
    }
    FCTSetDynamicProperty* DynamicProperty = new FCTSetDynamicProperty(SetProperty);
    DynamicProperty->InitProperty(SetProperty);
    GTempalteDynamicPropertyMap[KeyTypeName] = DynamicProperty;

    return DynamicProperty;
}

void ReleaseTempalteProperty()
{
	// 说明：UProperty对象不能释放，这个只能是全局管理的，由UE释放
	//ReleasePtrMap(GClassPropertyNameMap);
	GClassPropertyNameMap.clear(); // UE会自动释放，所以不能留

	ReleasePtrMap(GTempalteDynamicPropertyMap);
	ReleasePtrMap(GStructDynamicPropertyMap);
	ReleasePtrMap(GPropertyDynamicPropertyMap);
	ReleasePtrMap(GCppDynamicPropertyMap);

	GScriptStruct = nullptr;
}

void TArray_Clear(FScriptArray *ScriptArray, FProperty *Inner)
{
	int ElementSize = Inner->GetSize();
	int Numb = ScriptArray->Num();

	uint8 *ObjAddr = (uint8 *)ScriptArray->GetData();
	uint8 *ValueAddr = ObjAddr;
	for (int32 Index = 0; Index < Numb; ++Index)
	{
		ValueAddr = ObjAddr + Index * ElementSize;
		Inner->DestroyValue(ValueAddr);
	}
	ScriptArray->Remove(0, Numb, ElementSize);
}

void TMap_Clear(FScriptMap* ScriptMap, FMapProperty* MapProperty)
{
	FProperty* KeyProp = MapProperty->KeyProp;
	FProperty* ValueProp = MapProperty->ValueProp;
	const FScriptMapLayout &MapLayout = MapProperty->MapLayout;

	int32  MaxIndex = ScriptMap->GetMaxIndex();
	for(int32 PairIndex = 0; PairIndex < MaxIndex; ++PairIndex)
	{
		if(ScriptMap->IsValidIndex(PairIndex))
		{
			uint8* PairPtr = (uint8*)ScriptMap->GetData(PairIndex, MapLayout);
			uint8* Result  = PairPtr + MapLayout.ValueOffset;
			KeyProp->DestroyValue(PairPtr);
			ValueProp->DestroyValue(Result);
		}
	}
	ScriptMap->Empty(0, MapLayout);
}

void TSet_Clear(FScriptSet* ScriptMap, FSetProperty* SetProperty)
{
    FProperty* ElementProp = SetProperty->ElementProp;
    const FScriptSetLayout& SetLayout = SetProperty->SetLayout;
    int32  MaxIndex = ScriptMap->GetMaxIndex();
    for (int32 PairIndex = 0; PairIndex < MaxIndex; ++PairIndex)
    {
        if (ScriptMap->IsValidIndex(PairIndex))
        {
            uint8* Result = (uint8*)ScriptMap->GetData(PairIndex, SetLayout);
			ElementProp->DestroyValue(Result);
        }
    }
    ScriptMap->Empty(0, SetLayout);
}
