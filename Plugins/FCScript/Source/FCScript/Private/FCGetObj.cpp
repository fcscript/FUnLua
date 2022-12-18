#include "FCGetObj.h"
#include "FCTemplateType.h"
#include "FCCallScriptFunc.h"
#include "FCTArrayHelper.h"
#include "FCTMapHelper.h"
#include "FCTSetHelper.h"

FCGetObj* FCGetObj::s_Ins = nullptr;

FCGetObj::FCGetObj():m_nObjID(0)
{
	FCGetObj::s_Ins = this;

	m_CppProperty.ElementSize = sizeof(void*);
	m_CppProperty.m_ReadScriptFunc = ReadScriptCppPtr;
	m_CppProperty.m_WriteScriptFunc = PushScriptCppPtr;

	m_MapIteratorProperty.ElementSize = sizeof(TMapIterator);
	m_MapIteratorProperty.m_ReadScriptFunc = ReadScriptMapIterator;
	m_MapIteratorProperty.m_WriteScriptFunc = PushScriptMapIterator;
}

FCGetObj::~FCGetObj()
{
	if(FCGetObj::s_Ins == this)
	{
		FCGetObj::s_Ins = nullptr;
	}
}

void FCGetObj::Clear()
{
	while(m_IntPtrMap.size() >0)
	{
        CIntPtr2RefObjMap::iterator itObj = m_IntPtrMap.begin();
        FCObjRef* ObjRef = itObj->second;
		ObjRef->Ref = 1;
        ReleaseObjRef(ObjRef);
	}	
	m_nObjID = 0;
}

int64  FCGetObj::PushUObject(UObject *Obj)
{
	FCObjRef  *ObjRef = PushUObjectNoneRef(Obj);
	if(ObjRef)
	{
		++(ObjRef->Ref);
		return ObjRef->PtrIndex;
	}
	else
	{
		return 0;
	}
}

FCObjRef* FCGetObj::PushUObjectNoneRef(UObject* Obj)
{
	if (!Obj)
	{
		return nullptr;
	}
	ObjRefKey  ObjKey(nullptr, Obj);
	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		return itObj->second;
	}
	FCScriptContext* ScriptContext = GetScriptContext();
	FCDynamicClassDesc* ClassDesc = ScriptContext->RegisterUStruct(Obj->GetClass());

	FCObjRef* ObjRef = NewObjRef();
	ObjRef->Ref = 0;
	ObjRef->RefType = EFCObjRefType::RefObject;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = ClassDesc;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->ThisObjAddr = (uint8 *)Obj;
	m_ObjMap[ObjKey] = ObjRef;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
	return ObjRef;
}

int64  FCGetObj::PushNewObject(FCDynamicClassDesc* ClassDesc, const FName& Name, UObject* Outer)
{
	UClass  *Class = ClassDesc->m_Class;
	UObject* Obj = nullptr;

	FCObjRef* ObjRef = NewObjRef();
	ObjRef->Ref = 1;
	ObjRef->RefType = EFCObjRefType::NewUObject;
	//ObjRef->Parent = Outer;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = ClassDesc;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->ThisObjAddr = (uint8 *)Obj;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;

    // 后创建对象
#if ENGINE_MINOR_VERSION < 26
    Obj = StaticConstructObject_Internal(Class, Outer, Name);
#else
    FStaticConstructObjectParameters ObjParams(Class);
    ObjParams.Outer = Outer;
    ObjParams.Name = Name;
    Obj = StaticConstructObject_Internal(ObjParams);
#endif

    if (Obj)
    {
		ObjRefKey  ObjKey(nullptr, Obj);
        ObjRef->ThisObjAddr = (uint8 *)Obj;
        m_ObjMap[ObjKey] = ObjRef;
    }

	return ObjRef->PtrIndex;
}

int64  FCGetObj::PushNewStruct(FCDynamicClassDesc* ClassDesc)
{
	UStruct  *Struct = ClassDesc->m_Struct;
	int ValueSize = Struct->GetStructureSize();
	void *pValueAddr = NewStructBuffer(ValueSize);
	Struct->InitializeStruct(pValueAddr, 1);

	FCObjRef* ObjRef = NewObjRef();
	ObjRef->Ref = 1;
	ObjRef->RefType = EFCObjRefType::NewUStruct;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = ClassDesc;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->ThisObjAddr = (uint8 *)pValueAddr;
	ObjRef->DynamicProperty = GetStructDynamicProperty(Struct);

	ObjRefKey  ObjKey(nullptr, pValueAddr);
	m_ObjMap[ObjKey] = ObjRef;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
	return ObjRef->PtrIndex;
}

int64  FCGetObj::PushProperty(UObject *Parent, const FCDynamicProperty *DynamicProperty, void *pValueAddr)
{
	if(!Parent)
	{
		return 0;
	}
	FCObjRef *ParentRef = PushUObjectNoneRef(Parent);
	return PushChildProperty(ParentRef, DynamicProperty, pValueAddr);
}

int64  FCGetObj::PushChildProperty(FCObjRef* Parent, const FCDynamicProperty* DynamicProperty, void* pValueAddr)
{
	uint8  *ParentAddr = Parent->GetKeyAddr();
	ObjRefKey  ObjKey(ParentAddr, (const void*)(pValueAddr));

	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		++(itObj->second->Ref);
		return itObj->second->PtrIndex;
	}

    FFieldClass* FieldClass = DynamicProperty->Property->GetClass();
    uint64 CastFlags = FieldClass->GetCastFlags();
    uint64 AllFlags = CASTCLASS_FStructProperty;
    FCDynamicClassDesc* ClassDesc = nullptr;
    FCScriptContext* ScriptContext = GetScriptContext();
    if((CastFlags & AllFlags) != 0)
    {
        FStructProperty* StructProperty = (FStructProperty*)DynamicProperty->Property;
        ClassDesc = ScriptContext->RegisterUStruct(StructProperty->Struct);
    }
    else
    {
        ClassDesc = ScriptContext->RegisterByProperty((FProperty*)DynamicProperty->Property);
    }

	FCObjRef* ObjRef = NewObjRef();
	ObjRef->Ref = 1;
	ObjRef->RefType = EFCObjRefType::RefProperty;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = ClassDesc;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->DynamicProperty = (FCDynamicProperty*)DynamicProperty;
	ObjRef->ThisObjAddr = (uint8*)pValueAddr;
	m_ObjMap[ObjKey] = ObjRef;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;

	++(Parent->Ref);

	ObjRef->Parent = Parent;
	Parent->Childs.push_back(ObjRef);
	return ObjRef->PtrIndex;
}

int64  FCGetObj::PushStructValue(const FCDynamicProperty *DynamicProperty, void *pValueAddr)
{
	ObjRefKey  ObjKey(nullptr, pValueAddr);
	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		++(itObj->second->Ref);
		return itObj->second->PtrIndex;
	}
	void *SrcValueAddr = pValueAddr;
	// FStructPropertyDesc
	// 这种没有爹的，就拷贝一个吧
	FStructProperty* StructProperty = (FStructProperty *)DynamicProperty->Property;
	UStruct* Struct = StructProperty->Struct;
    int ValueSize = Struct->GetStructureSize();
    int ArrayDim = StructProperty->ArrayDim;
	pValueAddr = NewStructBuffer(ValueSize);
	Struct->InitializeStruct(pValueAddr, ArrayDim);
    StructProperty->CopyValuesInternal(pValueAddr, SrcValueAddr, ArrayDim);
	//StructProperty->CopyScriptStruct(pValueAddr, SrcValueAddr, ArrayDim);
	
	ObjKey = ObjRefKey(nullptr, pValueAddr);
	FCScriptContext *ScriptContext = GetScriptContext();
	FCDynamicClassDesc  *ClassDesc = ScriptContext->RegisterUStruct(Struct);

	FCObjRef  *ObjRef = NewObjRef();
	ObjRef->Ref = 1;
	ObjRef->RefType = EFCObjRefType::RefStructValue;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = ClassDesc;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->DynamicProperty = (FCDynamicProperty *)DynamicProperty;
	ObjRef->ThisObjAddr = (uint8* )pValueAddr;
	m_ObjMap[ObjKey] = ObjRef;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
	return ObjRef->PtrIndex;
}

int64  FCGetObj::PushNewTArray(const FCDynamicProperty* DynamicProperty, void* pValueAddr)
{
    FArrayProperty* ArrayProperty = (FArrayProperty*)DynamicProperty->Property;

    FScriptArray* DesContent = new FScriptArray();
    int64 ObjID = PushTemplate(DynamicProperty, DesContent, EFCObjRefType::NewTArray);

    FScriptArray* SrcContent = (FScriptArray*)pValueAddr;
    // 拷贝吧
    FCTArrayHelper  ArrayHelper(DesContent, DynamicProperty);
    ArrayHelper.Copy(SrcContent);

    return ObjID;
}

int64  FCGetObj::PushNewTMap(const FCDynamicProperty* DynamicProperty, void* pValueAddr)
{
    FMapProperty* MapProperty = (FMapProperty*)DynamicProperty->Property;

    FScriptMap* DesContent = new FScriptMap();
    int64 ObjID = PushTemplate(DynamicProperty, DesContent, EFCObjRefType::NewTMap);

    FScriptMap* SrcContent = (FScriptMap*)pValueAddr;

    FCTMapHelper  MapHelper(DesContent, DynamicProperty);
    MapHelper.Copy(SrcContent);

    return ObjID;
}

int64  FCGetObj::PushNewTSet(const FCDynamicProperty* DynamicProperty, void* pValueAddr)
{
    FSetProperty* SetProperty = (FSetProperty*)DynamicProperty->Property;

    FScriptSet* DesContent = new FScriptSet();
    int64 ObjID = PushTemplate(DynamicProperty, DesContent, EFCObjRefType::NewTSet);

    FScriptSet* SrcContent = (FScriptSet*)pValueAddr;

    FCTSetHelper  Helper(DesContent, DynamicProperty);
    Helper.Copy(SrcContent);

    return ObjID;
}

int64  FCGetObj::PushCppPropery(const FCDynamicProperty* DynamicProperty, void* pValueAddr)
{
	ObjRefKey  ObjKey(nullptr, pValueAddr);
	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		++(itObj->second->Ref);
		return itObj->second->PtrIndex;
	}
    void* SrcValueAddr = pValueAddr;
    // FStructPropertyDesc
    // 这种没有爹的，就拷贝一个吧
    const FProperty* Property = DynamicProperty->Property;
    int ValueSize = Property->ElementSize;
    int ArrayDim = Property->ArrayDim;
    pValueAddr = NewStructBuffer(ValueSize);
    Property->InitializeValue(pValueAddr);
    Property->CopySingleValue(pValueAddr, SrcValueAddr);


    FCScriptContext* ScriptContext = GetScriptContext();

    FCObjRef* ObjRef = NewObjRef();
    ObjRef->Ref = 1;
    ObjRef->RefType = EFCObjRefType::NewProperty;
    ObjRef->Parent = nullptr;
    ObjRef->ClassDesc = nullptr;
    ObjRef->PtrIndex = ++m_nObjID;
    ObjRef->DynamicProperty = (FCDynamicProperty*)DynamicProperty;
    ObjRef->ThisObjAddr = (uint8*)pValueAddr;
    m_ObjMap[ObjKey] = ObjRef;
    m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
    return ObjRef->PtrIndex;
}

int64  FCGetObj::PushTemplate(const FCDynamicProperty *DynamicProperty, void *pValueAddr, EFCObjRefType RefType)
{	
	ObjRefKey  ObjKey(nullptr, pValueAddr);
	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		++(itObj->second->Ref);
		return itObj->second->PtrIndex;
	}

	FCScriptContext *ScriptContext = GetScriptContext();

	FCObjRef  *ObjRef = NewObjRef();
	ObjRef->Ref = 1;
	ObjRef->RefType = RefType;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = nullptr;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->DynamicProperty = (FCDynamicProperty *)DynamicProperty;
	ObjRef->ThisObjAddr = (uint8 *)pValueAddr;
	m_ObjMap[ObjKey] = ObjRef;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
	return ObjRef->PtrIndex;
}

int64  FCGetObj::PushCppPtr(void* CppPtr)
{
	ObjRefKey  ObjKey(nullptr, CppPtr);
	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		++(itObj->second->Ref);
		return itObj->second->PtrIndex;
	}

	FCScriptContext* ScriptContext = GetScriptContext();

	FCObjRef* ObjRef = NewObjRef();
	ObjRef->Ref = 1;
	ObjRef->RefType = EFCObjRefType::CppPtr;
	ObjRef->Parent = nullptr;
	ObjRef->ClassDesc = nullptr;
	ObjRef->PtrIndex = ++m_nObjID;
	ObjRef->DynamicProperty = &m_CppProperty;
	ObjRef->ThisObjAddr = (uint8*)CppPtr;
	m_ObjMap[ObjKey] = ObjRef;
	m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
	return ObjRef->PtrIndex;
}

int64  FCGetObj::PushMapIterator(void* IteratorPtr)
{
    ObjRefKey  ObjKey(nullptr, IteratorPtr);
    CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
    if (itObj != m_ObjMap.end())
    {
        ++(itObj->second->Ref);
        return itObj->second->PtrIndex;
    }

    FCScriptContext* ScriptContext = GetScriptContext();

    FCObjRef* ObjRef = NewObjRef();
    ObjRef->Ref = 1;
    ObjRef->RefType = EFCObjRefType::MapIterator;
    ObjRef->Parent = nullptr;
    ObjRef->ClassDesc = nullptr;
    ObjRef->PtrIndex = ++m_nObjID;
    ObjRef->DynamicProperty = &m_MapIteratorProperty;
    ObjRef->ThisObjAddr = (uint8*)IteratorPtr;
    m_ObjMap[ObjKey] = ObjRef;
    m_IntPtrMap[ObjRef->PtrIndex] = ObjRef;
    return ObjRef->PtrIndex;
}

void   FCGetObj::NotifyDeleteUObject(const class UObjectBase* Object, int32 Index)
{
	ObjRefKey  ObjKey(nullptr, Object);
	CScriptRefObjMap::iterator itObj = m_ObjMap.find(ObjKey);
	if (itObj != m_ObjMap.end())
	{
		FCObjRef *ObjRef = itObj->second;
		DestroyChildRef(ObjRef);
	}
}

void   FCGetObj::DeleteValue(int64 ObjID)
{
	ReleaseValue(ObjID);
}

void   FCGetObj::ReleaseValue(int64 ObjID)
{
	CIntPtr2RefObjMap::iterator itObj = m_IntPtrMap.find(ObjID);
	if (itObj != m_IntPtrMap.end())
	{
		FCObjRef* ObjRef = itObj->second;
		ReleaseObjRef(ObjRef);
	}
}

void   FCGetObj::ReleaseCacheRef(int64 ObjID, int nCacheRef)
{
	CIntPtr2RefObjMap::iterator itObj = m_IntPtrMap.find(ObjID);
	if (itObj != m_IntPtrMap.end())
	{
		FCObjRef* ObjRef = itObj->second;
		ObjRef->Ref -= nCacheRef - 1;
		ReleaseObjRef(ObjRef);
	}
}

FCObjRef  *FCGetObj::NewObjRef()
{
	return new FCObjRef();
}

void  FCGetObj::ReleaseObjRef(FCObjRef* ObjRef)
{
    ObjRef->Ref--;
    if (ObjRef->Ref <= 0)
    {
        FCObjRef* ParentRef = ObjRef->Parent;
        if (ParentRef)
        {
            ParentRef->Childs.erase(ParentRef->Childs.MakeIterator(ObjRef));
        }
        DestroyChildRef(ObjRef);

		// 减少父对象的引用计数(递归)
		if(ParentRef)
		{
			ReleaseObjRef(ParentRef);
		}
    }
}

void  FCGetObj::DestroyChildRef(FCObjRef* ObjRef)
{
	// 父节点没有，儿子也没有必要留下, 儿子的儿子也不能留, 斩草要除根, 一脉不留存
	while (ObjRef->Childs.size() > 0)
	{
		FCObjRef* ChildPtr = ObjRef->Childs.front_ptr();
		ObjRef->Childs.pop_front();
		DestroyChildRef(ChildPtr);
	}
	DestroyObjRef(ObjRef);
}

void  FCGetObj::DestroyObjRef(FCObjRef *ObjRef)
{
	if(ObjRef)	
	{
		ObjRefKey ObjKey = ObjRef->GetRefKey();
		m_ObjMap.erase(ObjKey);
		m_IntPtrMap.erase(ObjRef->PtrIndex);

		switch(ObjRef->RefType)
		{
			case EFCObjRefType::NewUObject:
			{
			}
			break;
			case EFCObjRefType::NewUStruct:
			case EFCObjRefType::RefStructValue:
			{
				UStruct* Struct = ObjRef->ClassDesc->m_Struct;
				UScriptStruct *ScriptStruct = ObjRef->ClassDesc->m_ScriptStruct;
				int ValueSize = Struct->GetStructureSize();
				int ArrayDim = ObjRef->DynamicProperty ? ObjRef->DynamicProperty->Property->ArrayDim : 1;
				if (ScriptStruct)
				{
					if ((ScriptStruct->StructFlags & (STRUCT_IsPlainOldData | STRUCT_NoDestructor)))
					{
						break;
					}
				}
				Struct->DestroyStruct(ObjRef->GetPropertyAddr(), ArrayDim);
			}
			break;
			case EFCObjRefType::NewProperty:
			{
				const FProperty *Property = ObjRef->DynamicProperty->Property;
				Property->DestroyValue(ObjRef->GetThisAddr());
			}
			break;
			case EFCObjRefType::NewTArray:
			{
				FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
				FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
				FProperty *Inner = ArrayProperty->Inner;
				TArray_Clear(ScriptArray, Inner);
				delete ScriptArray;
			}
			break;
			case EFCObjRefType::NewTMap:
			{
				FScriptMap *ScriptMap = (FScriptMap*)ObjRef->GetThisAddr();
				TMap_Clear(ScriptMap, (FMapProperty *)ObjRef->DynamicProperty->Property);
				delete ScriptMap;
			}
            break;
            case EFCObjRefType::NewTSet:
            {
				FScriptSet* ScriptMap = (FScriptSet*)ObjRef->GetThisAddr();
				TSet_Clear(ScriptMap, (FSetProperty *)ObjRef->DynamicProperty->Property);
                delete ScriptMap;
            }
            break;
			case EFCObjRefType::MapIterator:
			{
				TMapIterator*pIteratorBuffer = (TMapIterator*)ObjRef->GetThisAddr();
				if(pIteratorBuffer)
				{
					delete pIteratorBuffer;
				}
			}
			break;
			default:
			break;
		}

		delete ObjRef;
	}
}

void  *FCGetObj::NewStructBuffer(int nBuffSize)
{
	return new char[nBuffSize];
}

void   FCGetObj::DelStructBuffer(void *pBuffer)
{
	if(pBuffer)
	{
		char *pBuf = (char *)pBuffer;
		delete []pBuf;
	}
}

static FCGetObj   s_GEtObjIns;
