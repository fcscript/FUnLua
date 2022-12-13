#include "FCTestB.h"

#include "FCObjectManager.h"
#include "FCGetObj.h"

//--------------------------测试代码-------------------------------------
FCDynamicProperty* GetObjAttrib(FCObjRef*& ObjRef, int64 ObjID, const char* AttribName)
{
	ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
	if (!ObjRef)
	{
		return nullptr;
	}
	FCDynamicClassDesc* ClassDesc = ObjRef->ClassDesc;
	FCDynamicProperty* DynamicProperty = nullptr;
	if (ObjRef->RefType == EFCObjRefType::NewUObject)
	{
		DynamicProperty = ClassDesc->FindAttribByName(AttribName);
	}
	else if (ObjRef->RefType == EFCObjRefType::NewUStruct)
	{
		DynamicProperty = ClassDesc->FindAttribByName(AttribName);
	}
	return DynamicProperty;
}

uint8* GetObjAttribAddr(int64 ObjID, const char* AttribName)
{
	FCObjRef* ObjRef = nullptr;
	FCDynamicProperty* DynamicProperty = GetObjAttrib(ObjRef, ObjID, AttribName);
	if (DynamicProperty)
	{
		uint8* ObjAddr = (uint8*)(ObjRef->GetPropertyAddr());
		uint8* ValueAddr = ObjAddr + DynamicProperty->Offset_Internal;
		return ValueAddr;
	}
	return nullptr;
}

template <class _Ty>
void   WriteAny(int64 ObjID, const char* AttribName, const _Ty& v)
{
	uint8* ValueAddr = GetObjAttribAddr(ObjID, AttribName);
	if (ValueAddr)
	{
		*((_Ty*)ValueAddr) = v;
	}
}

void   WriteVec3(int64 ObjID, const char* AttribName, const FVector& v)
{
	WriteAny(ObjID, AttribName, v);
}

void   WriteUObject(int64 ObjID, const char* AttribName, UObject* v)
{
	FCObjRef* ObjRef = nullptr;
	FCDynamicProperty* DynamicProperty = GetObjAttrib(ObjRef, ObjID, AttribName);
	if (DynamicProperty)
	{
		uint8* ObjAddr = (uint8*)(ObjRef->GetPropertyAddr());
		if (!ObjAddr)
		{
			return;
		}
		uint8* ValueAddr = ObjAddr + DynamicProperty->Offset_Internal;
		UStruct* Struct = DynamicProperty->Property->GetOwnerStruct();
		UClass* InClass = UObject::StaticClass();
		if (Struct == InClass || Struct->IsChildOf(InClass))
		{
			*((UObject**)ValueAddr) = v;
		}
		else
		{
			*((UObject**)ValueAddr) = nullptr;
		}
	}
}
void   WriteIntArray(int64 ObjID, const char* AttribName, int* aInt, int Numb)
{
	uint8* ValueAddr = GetObjAttribAddr(ObjID, AttribName);
	if (ValueAddr)
	{
		memcpy(ValueAddr, aInt, sizeof(int) * Numb);
	}
}

template <class _Ty>
void   ReadAny(int64 ObjID, const char* AttribName, _Ty& v)
{
	uint8* ValueAddr = GetObjAttribAddr(ObjID, AttribName);
	if (ValueAddr)
	{
		v = *((_Ty*)ValueAddr);
	}
}

void   ReadVec3(int64 ObjID, const char* AttribName, FVector& v)
{
	ReadAny(ObjID, AttribName, v);
}

void   ReadUObject(int64 ObjID, const char* AttribName, UObject*& v)
{
	FCObjRef* ObjRef = nullptr;
	FCDynamicProperty* DynamicProperty = GetObjAttrib(ObjRef, ObjID, AttribName);
	if (DynamicProperty)
	{
		uint8* ObjAddr = (uint8*)(ObjRef->GetPropertyAddr());
		if (!ObjAddr)
		{
			v = nullptr;
			return;
		}
		uint8* ValueAddr = ObjAddr + DynamicProperty->Offset_Internal;
		UStruct* Struct = DynamicProperty->Property->GetOwnerStruct();
		if (v)
		{
			if (Struct == v->GetClass() || Struct->IsChildOf(v->GetClass()))
			{
				v = *((UObject**)ValueAddr);
			}
			else
			{
				v = nullptr;
			}
		}
		else
		{
			v = nullptr;
		}
	}
}

void  BindScriptCallback(int64 ObjID, const char* AttribName)
{
	FCObjRef* ObjRef = nullptr;
	FCDynamicProperty* DynamicProperty = GetObjAttrib(ObjRef, ObjID, AttribName);
	if (DynamicProperty)
	{
	}
}

void  PushStringToArray(int64 ObjID, const char* AttribName, const char* InStr)
{
	FString InName = UTF8_TO_TCHAR(InStr);
	FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
	if (ObjRef)
	{
		//FArrayProperty  *ArrayProperty = 
		int ElementSize = sizeof(FString);
		FScriptArray* ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
		int Index = ScriptArray->Num();
		ScriptArray->Add(1, ElementSize);
		uint8* ObjAddr = (uint8*)ScriptArray->GetData();
		uint8* ValudAddr = ObjAddr + Index * ElementSize;
		new(ValudAddr) FString(InName);
	}
}
