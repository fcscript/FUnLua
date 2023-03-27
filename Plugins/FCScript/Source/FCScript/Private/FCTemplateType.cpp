#include "FCTemplateType.h"


template<class _Ty>
_Ty* NewUEProperty(UScriptStruct* ScriptStruct)
{
#if OLD_UE_ENGINE
    _Ty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) _Ty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash);
#elif ENGINE_MAJOR_VERSION >= 5 
    _Ty* Property = new _Ty(ScriptStruct, NAME_None, RF_Transient);
    Property->PropertyFlags = CPF_ZeroConstructor | CPF_IsPlainOldData | CPF_NoDestructor | CPF_HasGetValueTypeHash;
#else
	_Ty* Property = new _Ty(ScriptStruct, NAME_None, RF_Transient, 0, CPF_HasGetValueTypeHash);
#endif
    return Property;
}

FProperty  *NewUEBoolProperty(UScriptStruct* ScriptStruct)
{
#if OLD_UE_ENGINE
	// see overloaded operator new that defined in DECLARE_CLASS(...)
	UBoolProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UBoolProperty(FObjectInitializer(), EC_CppProperty, 0, (EPropertyFlags)0, 0xFF, 1, true);
#elif ENGINE_MAJOR_VERSION >= 5 
    constexpr auto Params = UECodeGen_Private::FBoolPropertyParams
    {
        nullptr,
        nullptr,
        CPF_None,
        UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool,
        RF_Transient,
        1,
        nullptr,
        nullptr,
        sizeof(bool),
        sizeof(ScriptStruct),
        nullptr,
        METADATA_PARAMS(nullptr, 0)
    };
    FBoolProperty* Property = new FBoolProperty(ScriptStruct, Params);
#else
	FBoolProperty* Property = new FBoolProperty(ScriptStruct, NAME_None, RF_Transient, 0, (EPropertyFlags)0, 0xFF, 1, true);
#endif
    return Property;
}

FProperty* NewUEStructProperty(UScriptStruct* Struct, UScriptStruct* ScriptStruct)
{
#if OLD_UE_ENGINE
	// see overloaded operator new that defined in DECLARE_CLASS(...)
    UStructProperty *Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UStructProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash, Struct);
#elif ENGINE_MAJOR_VERSION >= 5 
    const auto Params = UECodeGen_Private::FStructPropertyParams
    {
        nullptr,
        nullptr,
        ScriptStruct->GetCppStructOps()
            ? ScriptStruct->GetCppStructOps()->GetComputedPropertyFlags() | CPF_HasGetValueTypeHash
            : CPF_HasGetValueTypeHash,
        UECodeGen_Private::EPropertyGenFlags::Struct,
        RF_Transient,
        1,
        nullptr,
        nullptr,
        0,
        nullptr,
        METADATA_PARAMS(nullptr, 0)
    };
    FStructProperty* Property = new FStructProperty(ScriptStruct, Params);
    Property->Struct = Struct;
    Property->ElementSize = Struct->PropertiesSize;
#else
	FStructProperty* Property = new FStructProperty(ScriptStruct, NAME_None, RF_Transient, 0, CPF_HasGetValueTypeHash, Struct);
#endif
	return Property;
}

FProperty  *NewUEClassProperty(UClass *Class, UScriptStruct* ScriptStruct)
{
#if OLD_UE_ENGINE
	// see overloaded operator new that defined in DECLARE_CLASS(...)
    //UObjectProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UObjectProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash, Class);
    UClassProperty *Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) UClassProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_HasGetValueTypeHash | CPF_UObjectWrapper, Class, nullptr);
#elif ENGINE_MAJOR_VERSION >= 5 
    constexpr auto Params = UECodeGen_Private::FObjectPropertyParams
    {
        nullptr,
        nullptr,
        CPF_HasGetValueTypeHash,
        UECodeGen_Private::EPropertyGenFlags::Object,
        RF_Transient,
        1,
        nullptr,
        nullptr,
        0,
        nullptr,
        METADATA_PARAMS(nullptr, 0)
    };
    FObjectProperty* Property = new FObjectProperty(ScriptStruct, Params);
    Property->PropertyClass = Class;
#else
	FObjectProperty* Property = new FObjectProperty(ScriptStruct, NAME_None, RF_Transient, 0, CPF_HasGetValueTypeHash, Class);
#endif
	return Property;
}

FArrayProperty  *NewUEArrayProperty(UScriptStruct* ScriptStruct)
{
#if OLD_UE_ENGINE
	FArrayProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) FArrayProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_None);
#else
	FArrayProperty* Property = new FArrayProperty(ScriptStruct, NAME_None, RF_Transient);
#endif
	return Property;
}

FMapProperty  *NewUEMapProperty(UScriptStruct* ScriptStruct)
{	
#if OLD_UE_ENGINE
	FMapProperty* Property = new (EC_InternalUseOnlyConstructor, ScriptStruct, NAME_None, RF_Transient) FMapProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_None);
#else
	FMapProperty* Property = new FMapProperty(ScriptStruct, NAME_None, RF_Transient);
#endif
	return Property;
}

FSetProperty* NewUESetProperty(UScriptStruct* ScriptStruct)
{
#if OLD_UE_ENGINE
	FSetProperty* Property = new (EC_InternalUseOnlyConstructor, (UClass*)ScriptStruct, NAME_None, RF_Transient) FSetProperty(FObjectInitializer(), EC_CppProperty, 0, CPF_None);
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

typedef std::unordered_map<const char *, FCInnerBaseType, FCStringHash, FCStringEqual> CInnerTypeMap;
typedef std::unordered_map<const char *, FProperty*, FCStringHash, FCStringEqual> CTemplatePropertyNameMap;
typedef std::unordered_map<UStruct*, FCDynamicProperty*> CStructDynamicPropertyMap;
typedef std::unordered_map<FProperty*, FCDynamicProperty*> CPropertyDynamicPropertyMap;
typedef std::unordered_map<const char*, FCDynamicProperty*, FCStringHash, FCStringEqual> CCppDynamicPropertyMap;
typedef std::unordered_map<const FProperty*, bool> CPropertyBaseCopyTypeMap; // base type(bool, int8, int16, int32, float, double), can memory copy
CInnerTypeMap            GInnerTypeMap;
CTemplatePropertyNameMap GClassPropertyNameMap;
CStructDynamicPropertyMap GStructDynamicPropertyMap;
CPropertyDynamicPropertyMap GPropertyDynamicPropertyMap;
CCppDynamicPropertyMap      GCppDynamicPropertyMap;
CPropertyBaseCopyTypeMap    GPropertyBaseCopyTypeMap;

FCInnerBaseType GetInnerType(const char *InClassName)
{
    if(GInnerTypeMap.empty())
    {
        GInnerTypeMap["bool"] = FC_INNER_TYPE_bool;
        GInnerTypeMap["byte"] = FC_INNER_TYPE_INT8;
        GInnerTypeMap["uint8"] = FC_INNER_TYPE_INT8;
        GInnerTypeMap["int8"] = FC_INNER_TYPE_INT8;
        GInnerTypeMap["int16"] = FC_INNER_TYPE_INT16;
        GInnerTypeMap["uint16"] = FC_INNER_TYPE_UINT16;
        GInnerTypeMap["int"] = FC_INNER_TYPE_INT;
        GInnerTypeMap["int32"] = FC_INNER_TYPE_INT;
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

FProperty  *CreateClassProperty(const char *InClassName)
{
	CTemplatePropertyNameMap::iterator itProperty = GClassPropertyNameMap.find(InClassName);
	if(itProperty != GClassPropertyNameMap.end())
	{
		return itProperty->second;
	}
    FCInnerBaseType InnerType = GetInnerType(InClassName);
    if (InnerType != FC_INNER_TYPE_Unknow)
    {
        InClassName = GetInnerTypeName(InnerType);
        itProperty = GClassPropertyNameMap.find(InClassName);
        if (itProperty != GClassPropertyNameMap.end())
        {
            return itProperty->second;
        }
        FProperty* Property = CreateBaseProperty(InnerType);
        GClassPropertyNameMap[InClassName] = Property;
        return Property;
    }
	const FCDynamicClassDesc *DynamicClass = GetScriptContext()->RegisterUClass(InClassName);
	if(!DynamicClass)
	{
        // not surport type
		return nullptr;
	}
	if(!DynamicClass->m_Class)
	{
		if (DynamicClass->m_Struct)
		{
			// 注明一下，这里的Struct一定是UScriptStruct
			FProperty* Property = NewUEStructProperty((UScriptStruct *)DynamicClass->m_Struct, GetGlbScriptStruct());
			InClassName = DynamicClass->m_UEClassName;
			GClassPropertyNameMap[InClassName] = Property;
			return Property;
		}
		return nullptr;
	}
	FProperty *Property = NewUEClassProperty(DynamicClass->m_Class, GetGlbScriptStruct());
	InClassName = DynamicClass->m_UEClassName;
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
    DynamicPropery->Name = GetConstName(DynamicPropery->Name);
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
        DynamicPropery->Name = GetConstName(DynamicPropery->Name);
	}
	GPropertyDynamicPropertyMap[InProperty] = DynamicPropery;
	return DynamicPropery;
}

FCDynamicProperty* GetDynamicPropertyByCppType(FCPropertyType InType, const char* InClassName, int InElementSize)
{
    CCppDynamicPropertyMap::iterator itProperty = GCppDynamicPropertyMap.find(InClassName);
    if (itProperty != GCppDynamicPropertyMap.end())
    {
        return itProperty->second;
    }
    FCDynamicProperty* DynamicPropery = new FCDynamicProperty();
    DynamicPropery->InitCppType(InType, InClassName, InElementSize);
    GCppDynamicPropertyMap[InClassName] = DynamicPropery;
    return DynamicPropery;
}

FProperty *QueryTempalteProperty(const char* InClassName)
{
	FProperty *Property = CreateClassProperty(InClassName);
	return Property;
}

FArrayProperty* CreateTArrayProperty(const char* InPropertyType)
{
	FProperty *Property = QueryTempalteProperty(InPropertyType);
	if(!Property)
	{
		return nullptr;
	}
	FArrayProperty *ArrayProperty = NewUEArrayProperty(GetGlbScriptStruct());
	ArrayProperty->Inner = Property;	
	return ArrayProperty;
}

typedef std::unordered_map<const char *, FCDynamicProperty*, FCStringHash, FCStringEqual> CTempalteDynamicPropertyMap;
typedef std::unordered_map<FCDoubleKey, FCDynamicProperty*> CMapTempalteDynamicPropertyMap;
CTempalteDynamicPropertyMap   GTArrayDynamicPropertyMap;
CMapTempalteDynamicPropertyMap GTMapDynamicPropertyMap;
CTempalteDynamicPropertyMap    GTSetDynamicPropertyMap;

FCDynamicProperty* GetTArrayDynamicProperty(const char* InPropertyType)
{
	CTempalteDynamicPropertyMap::iterator itProperty = GTArrayDynamicPropertyMap.find(InPropertyType);
	if(itProperty != GTArrayDynamicPropertyMap.end())
	{
		return itProperty->second;
	}
    InPropertyType = GetConstName(InPropertyType);
	FArrayProperty* ArrayProperty = CreateTArrayProperty(InPropertyType);
	if(!ArrayProperty)
	{
        GTArrayDynamicPropertyMap[InPropertyType] = nullptr;
		return nullptr;
	}
	FCTArrayDynamicProperty  *DynamicProperty = new FCTArrayDynamicProperty(ArrayProperty, GetDynamicPropertyByUEProperty(ArrayProperty->Inner));
	DynamicProperty->InitProperty(ArrayProperty);
    GTArrayDynamicPropertyMap[InPropertyType] = DynamicProperty;
	return DynamicProperty;
}

//---------------------------------------------------------------------

FMapProperty* CreateTMapProperty(const char *KeyType, const char *ValueType)
{
	FProperty *KeyProperty = QueryTempalteProperty(KeyType);
	FProperty *ValueProperty = QueryTempalteProperty(ValueType);
	if(!KeyProperty || !ValueProperty)
	{
		return nullptr;
	}
	FMapProperty  *MapProperty = NewUEMapProperty(GetGlbScriptStruct());
	MapProperty->KeyProp = KeyProperty;
	MapProperty->ValueProp = ValueProperty;

	int ValueSize = ValueProperty->ElementSize * ValueProperty->ArrayDim;
	int AlignKeySize = KeyProperty->GetMinAlignment();
    int AlignValueSize = ValueProperty->GetMinAlignment();

	MapProperty->MapLayout = FScriptMap::GetScriptLayout(KeyProperty->ElementSize, AlignKeySize, ValueSize, AlignValueSize);

	return MapProperty;
}

FCDynamicProperty *GetTMapDynamicProperty(const char* KeyTypeName, const char* ValueTypeName)
{
	FCDoubleKey  MapKey(KeyTypeName, ValueTypeName);
	CMapTempalteDynamicPropertyMap::iterator itProperty = GTMapDynamicPropertyMap.find(MapKey);
	if(itProperty != GTMapDynamicPropertyMap.end())
	{
		return itProperty->second;
	}
	KeyTypeName = GetConstName(KeyTypeName);
	ValueTypeName = GetConstName(ValueTypeName);
	MapKey = FCDoubleKey(KeyTypeName, ValueTypeName);

	FMapProperty  *MapProperty = CreateTMapProperty(KeyTypeName, ValueTypeName);
	if(!MapProperty)
	{
        GTMapDynamicPropertyMap[MapKey] = nullptr;
		return nullptr;
	}
	FCTMapDynamicProperty  *DynamicProperty = new FCTMapDynamicProperty(MapProperty);
	DynamicProperty->InitProperty(MapProperty);
    GTMapDynamicPropertyMap[MapKey] = DynamicProperty;

	return DynamicProperty;
}

//-----------------------------------------------
FSetProperty* CreateTSetProperty(const char* ClassName)
{
    FProperty* ElementProp = QueryTempalteProperty(ClassName);
    if (!ElementProp)
    {
        return nullptr;
    }
    FSetProperty* SetProperty = NewUESetProperty(GetGlbScriptStruct());
	SetProperty->ElementProp = ElementProp;

    int ValueSize = ElementProp->ElementSize * ElementProp->ArrayDim;
    int AlignValueSize = ElementProp->GetMinAlignment();

	SetProperty->SetLayout = FScriptSet::GetScriptLayout(ValueSize, AlignValueSize);

    return SetProperty;
}

FCDynamicProperty* GetTSetDynamicProperty(const char* KeyTypeName)
{
    // 说明，由于TMap与TArray的参数不一样，所以不会存在相同的TemplateID, 这里共用一个模板列表吧
    CTempalteDynamicPropertyMap::iterator itProperty = GTSetDynamicPropertyMap.find(KeyTypeName);
    if (itProperty != GTSetDynamicPropertyMap.end())
    {
        return itProperty->second;
    }
	KeyTypeName = GetConstName(KeyTypeName);
    FSetProperty* SetProperty = CreateTSetProperty(KeyTypeName);
    if (!SetProperty)
    {
        GTSetDynamicPropertyMap[KeyTypeName] = nullptr;
        return nullptr;
    }
    FCTSetDynamicProperty* DynamicProperty = new FCTSetDynamicProperty(SetProperty);
    DynamicProperty->InitProperty(SetProperty);
    GTSetDynamicPropertyMap[KeyTypeName] = DynamicProperty;

    return DynamicProperty;
}

void ReleaseTempalteProperty()
{
	// 说明：UProperty对象不能释放，这个只能是全局管理的，由UE释放
	//ReleasePtrMap(GClassPropertyNameMap);
	GClassPropertyNameMap.clear(); // UE会自动释放，所以不能留

	ReleasePtrMap(GTArrayDynamicPropertyMap);
    ReleasePtrMap(GTSetDynamicPropertyMap);
    ReleasePtrMap(GTMapDynamicPropertyMap);
	ReleasePtrMap(GStructDynamicPropertyMap);
	ReleasePtrMap(GPropertyDynamicPropertyMap);
	ReleasePtrMap(GCppDynamicPropertyMap);

    GPropertyBaseCopyTypeMap.clear();

	GScriptStruct = nullptr;
}

template <class _TyMapList>
void SetMapPtrRefFlags(_TyMapList &MapList)
{
    for (_TyMapList::iterator it = MapList.begin(); it != MapList.end(); ++it)
    {
        SetPtrRefFlag(it->second->SafePropertyPtr);
    }
}

void SetAllCppPropertyRefFlag()
{
    SetMapPtrRefFlags(GTArrayDynamicPropertyMap);
    SetMapPtrRefFlags(GTMapDynamicPropertyMap);
    SetMapPtrRefFlags(GTSetDynamicPropertyMap);

    SetMapPtrRefFlags(GStructDynamicPropertyMap);
    SetMapPtrRefFlags(GPropertyDynamicPropertyMap);
    SetMapPtrRefFlags(GCppDynamicPropertyMap);
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
    ScriptArray_Remove(ScriptArray, 0, Numb, ElementSize);
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

bool IsBaseTypeCopy(const FProperty *InPropery)
{
    CPropertyBaseCopyTypeMap::iterator itFind = GPropertyBaseCopyTypeMap.find(InPropery);
    if(itFind != GPropertyBaseCopyTypeMap.end())
    {
        return itFind->second;
    }
    FFieldClass *FieldClass = InPropery->GetClass();
    uint64 CastFlags = FieldClass->GetCastFlags();
    uint64 AllBaseFlags = CASTCLASS_FInt8Property
        | CASTCLASS_FByteProperty
        | CASTCLASS_FBoolProperty
        | CASTCLASS_FInt16Property
        | CASTCLASS_FUInt16Property
        | CASTCLASS_FIntProperty
        | CASTCLASS_FUInt32Property
        | CASTCLASS_FInt64Property
        | CASTCLASS_FUInt64Property
        | CASTCLASS_FFloatProperty
        | CASTCLASS_FDoubleProperty;
    bool bBaseType = (CastFlags & AllBaseFlags) != 0;
    GPropertyBaseCopyTypeMap[InPropery] = bBaseType;
    return bBaseType;
}