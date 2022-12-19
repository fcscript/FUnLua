#include "FCCallScriptFunc.h"
#include "UObject/UnrealType.h"
#include "Templates/Casts.h"
#include "Engine/Level.h"
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FCBrigeBase.h"
#include "../LuaCore/LuaContext.h"

#include "FCGetObj.h"

//---------------------------------------------------------------------------------
void  PushScriptDefault(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	// 什么也不做，进到这里也许是一个错误，不支持的类型噢
    lua_pushnil(L);
}
void  PushScriptBool(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	lua_pushboolean(L, *((bool*)ValueAddr));
}
void  PushScriptInt8(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	lua_pushinteger(L, *((int8*)ValueAddr));
}
void  PushScriptInt16(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    lua_pushinteger(L, *((short*)ValueAddr));
}
void  PushScriptInt32(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    lua_pushinteger(L, *((int32*)ValueAddr));
}
void  PushScriptInt64(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    lua_pushinteger(L, *((int64*)ValueAddr));
}
void  PushScriptFloat(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	lua_pushnumber(L, *((float*)ValueAddr));
}
void  PushScriptDouble(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    lua_pushnumber(L, *((double*)ValueAddr));
}
void  PushScriptFName(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	FString value = ((FName*)ValueAddr)->ToString();
	//FTCHARToUTF8 toUtf8(*value);
	//lua_pushlstring(L, toUtf8.Get(), toUtf8.Length());
    lua_pushstring(L, TCHAR_TO_UTF8(*value));
}
void  PushScriptFString(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FString &value = *((FString*)ValueAddr);
    //FTCHARToUTF8 toUtf8(*value);
    //lua_pushlstring(L, toUtf8.Get(), toUtf8.Length());
    lua_pushstring(L, TCHAR_TO_UTF8(*value));
}
void  PushScriptFText(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	FString value = ((FText*)ValueAddr)->ToString();
	//FTCHARToUTF8 toUtf8(*value);
	//lua_pushlstring(L, toUtf8.Get(), toUtf8.Length());
    lua_pushstring(L, TCHAR_TO_UTF8(*value));
}
void  PushScriptFVector(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	LuaPushValue(L, *((const FVector*)ValueAddr), false);
}
void  PushScriptFVector2D(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    LuaPushValue(L, *((const FVector2D*)ValueAddr), false);
}
void  PushScriptFVector4D(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    LuaPushValue(L, *((const FVector4*)ValueAddr), false);
}

// 功能：根据脚本变量得到UE对象的描述类
FCDynamicClassDesc* GetScriptValueClassDesc(lua_State* L, int Idx)
{
	int Type = lua_type(L, Idx);
	if (Type == LUA_TTABLE)
	{

	}
	else if (Type == LUA_TUSERDATA)
	{
		int64 ObjID = (int64)lua_touserdata(L, Idx);
		FCObjRef *ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
		if (ObjRef && ObjRef->IsValid())
		{
			return ObjRef->ClassDesc;
		}
	}
	return nullptr;
}

bool  IsCanCastToScript(lua_State* L, int Idx, const FCDynamicPropertyBase *DynamicProperty)
{
	FCDynamicClassDesc *ClassDesc = GetScriptValueClassDesc(L, Idx);
	if(!ClassDesc)
	{
		return false;
	}

	UStruct *PropertyStruct = DynamicProperty->Property->GetOwnerStruct();
	if( ClassDesc->m_Struct != PropertyStruct)
	{
		// 必须是子类(这个地方如果会慢的话，可以用hash_map优化)
		if(PropertyStruct->IsChildOf(ClassDesc->m_Struct))
		{
			return true;
		}

		const char* Name = GetUEClassName(ClassDesc->m_UEClassName.c_str());
		if (DynamicProperty->Name == Name)
		{
			return true;
		}
		return false;
	}
	return true;
}

// 将UE对象写入脚本对象
void  PushScriptStruct(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FStructProperty* StructProperty = (FStructProperty*)DynamicProperty->Property;
	// lua 里面没有强类型，所以这里就直接写入吧
	{
		int64 ObjID = 0;
		{
			if (ThisObj)
			{
				ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
			}
			else
			{
				FCObjRef  *ParentRef = (FCObjRef *)ObjRefPtr;
				if(ParentRef)
					ObjID = FCGetObj::GetIns()->PushChildProperty(ParentRef, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
				else
					ObjID = FCGetObj::GetIns()->PushStructValue((const FCDynamicProperty*)DynamicProperty, ValueAddr);
			}
			FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
		}
	}
}

void  PushScriptUObject(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	UObject* value = *((UObject**)ValueAddr);
	FCScript::PushUObject(L, value);
}

void  PushScriptCppPtr(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	int64 ObjID = FCGetObj::GetIns()->PushCppPtr(ValueAddr);
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptMapIterator(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	int64 ObjID = FCGetObj::GetIns()->PushMapIterator(ValueAddr);
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptWeakObject(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FWeakObjectPtr *WeakPtr = (FWeakObjectPtr *)ValueAddr;
	int64 ObjID = 0;
	if(ThisObj)
	{
		ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	else
	{
		ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)DynamicProperty, ValueAddr, EFCObjRefType::NewTWeakPtr);
	}
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptLazyObject(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FLazyObjectPtr *LazyPtr = (FLazyObjectPtr *)ValueAddr;
	int64 ObjID = 0;
	if(ThisObj)
	{
		ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	else
	{
		ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)DynamicProperty, ValueAddr, EFCObjRefType::NewTLazyPtr);
	}
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptDelegate(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FStructProperty* StructProperty = (FStructProperty*)DynamicProperty->Property;

	int64 ObjID = 0;
	// 如果变量是UObject的属性
	if (ThisObj)
	{
		ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	else
	{
		ObjID = FCGetObj::GetIns()->PushStructValue((const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptTArray(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	FArrayProperty* ArrayProperty = (FArrayProperty*)DynamicProperty->Property;
	int64 ObjID = 0;
	// 如果变量是UObject的属性
	if (ThisObj)
	{
		ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	else
	{
		ObjID = FCGetObj::GetIns()->PushNewTArray((const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptTMap(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FMapProperty* MapProperty = (FMapProperty*)DynamicProperty->Property;
    int64 ObjID = 0;
    // 如果变量是UObject的属性
    if (ThisObj)
    {
        ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else
    {
        ObjID = FCGetObj::GetIns()->PushNewTMap((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());    
}

void  PushScriptTSet(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSetProperty* MapProperty = (FSetProperty*)DynamicProperty->Property;
    int64 ObjID = 0;
    // 如果变量是UObject的属性
    if (ThisObj)
    {
        ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else
    {
        ObjID = FCGetObj::GetIns()->PushNewTSet((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  InitDynamicPropertyWriteFunc(FCDynamicProperty *DynamicProperty, FCPropertyType Flag)
{
	if(DynamicProperty->m_WriteScriptFunc)
	{
		return ;
	}
	DynamicProperty->m_WriteScriptFunc = PushScriptDefault;
	switch(Flag)
	{
		case FCPROPERTY_BoolProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptBool;
			break;
		case FCPROPERTY_Int8Property:
		case FCPROPERTY_ByteProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptInt8;
			break;
		case FCPROPERTY_Int16Property:
			DynamicProperty->m_WriteScriptFunc = PushScriptInt16;
			break;
		case FCPROPERTY_IntProperty:
		case FCPROPERTY_UInt32Property:
			DynamicProperty->m_WriteScriptFunc = PushScriptInt32;
			break;
		case FCPROPERTY_FloatProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptFloat;
			break;
		case FCPROPERTY_DoubleProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptDouble;
			break;
		case FCPROPERTY_Int64Property:
		case FCPROPERTY_UInt64Property:
			DynamicProperty->m_WriteScriptFunc = PushScriptInt64;
			break;
		case FCPROPERTY_NameProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptFName;
			break;
		case FCPROPERTY_StrProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptFString;
			break;
		case FCPROPERTY_TextProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptFText;
			break;
		case FCPROPERTY_ObjectProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptUObject;
			break;
		case FCPROPERTY_WeakObjectPtr:
			DynamicProperty->m_WriteScriptFunc = PushScriptWeakObject;
			break;
		case FCPROPERTY_LazyObjectPtr:
			DynamicProperty->m_WriteScriptFunc = PushScriptLazyObject;
			break;
		case FCPROPERTY_StructProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptStruct;
			break;
		case FCPROPERTY_Vector2:
			DynamicProperty->m_WriteScriptFunc = PushScriptFVector2D;
			break;
		case FCPROPERTY_Vector3:
			DynamicProperty->m_WriteScriptFunc = PushScriptFVector;
			break;
		case FCPROPERTY_Vector4:
			DynamicProperty->m_WriteScriptFunc = PushScriptFVector4D;
			break;
		case FCPROPERTY_Array:
			DynamicProperty->m_WriteScriptFunc = PushScriptTArray;
			break;
        case FCPROPERTY_Map:
            DynamicProperty->m_WriteScriptFunc = PushScriptTMap;
            break;
        case FCPROPERTY_Set:
            DynamicProperty->m_WriteScriptFunc = PushScriptTSet;
            break;
		case FCPROPERTY_DelegateProperty:
		case FCPROPERTY_MulticastDelegateProperty:
			DynamicProperty->m_WriteScriptFunc = PushScriptDelegate;
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------------

void  ReadScriptDefault(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
}

void  ReadScriptBool(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	*((bool*)ValueAddr) = lua_toboolean(L, ValueIdx) != 0;
}

void  ReadScriptInt8(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	*((uint8*)ValueAddr) = (uint8)lua_tointeger(L, ValueIdx);
}
void  ReadScriptInt16(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    *((uint16*)ValueAddr) = (uint16)lua_tointeger(L, ValueIdx);
}
void  ReadScriptInt32(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    *((uint32*)ValueAddr) = (uint32)lua_tointeger(L, ValueIdx);
}
void  ReadScriptInt64(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    *((int64*)ValueAddr) = (int64)lua_tointeger(L, ValueIdx);
}
void  ReadScriptFloat(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	*((float*)ValueAddr) = (float)lua_tonumber(L, ValueIdx);
}
void  ReadScriptDouble(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    *((double*)ValueAddr) = (double)lua_tonumber(L, ValueIdx);
}
void  ReadScriptFName(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	const char* StrParam = lua_tostring(L, ValueIdx);
	StrParam = StrParam ? StrParam : "";
	*((FName*)ValueAddr) = FName(UTF8_TO_TCHAR(StrParam));
}
void  ReadScriptFString(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	const char *StrParam = lua_tostring(L, ValueIdx);
	FString* value = (FString*)ValueAddr;
	if(StrParam)
	{
		*value = UTF8_TO_TCHAR(StrParam);
	}
	else
	{
		value->Empty();
	}
}
void  ReadScriptFText(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	const char* StrParam = lua_tostring(L, ValueIdx);
	FString InStr;
	if(StrParam)
		InStr = UTF8_TO_TCHAR(StrParam);
	FText* value = (FText*)ValueAddr;
	*value = FText::FromString(InStr);
}
// 将脚本对象写入到UE对象
void  ReadScriptStruct(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FStructProperty* StructProperty = (FStructProperty*)DynamicProperty->Property;
	FCObjRef* ObjRef = (FCObjRef *)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef && ObjRef->ClassDesc && ObjRef->ClassDesc->m_Struct == StructProperty->Struct)
	{
		StructProperty->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), StructProperty->ArrayDim);
	}
}
void  ReadScriptUObject(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    FObjectProperty* ObjectProperty = (FObjectProperty*)DynamicProperty->Property;
	UObject *SrcObj = FCScript::GetUObject(L, ValueIdx);
    UClass* InClass = SrcObj ? SrcObj->GetClass() : UObject::StaticClass();
    UClass* Class = ObjectProperty->PropertyClass;
    if (Class == InClass || InClass->IsChildOf(Class))
    {
        *((UObject**)ValueAddr) = SrcObj;
    }
    else
    {
        *((UObject**)ValueAddr) = nullptr;
    }
}

void  ReadScriptCppPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    // 理论上不能走到这里来，什么也不做吧
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if(ObjRef && ObjRef->RefType == EFCObjRefType::CppPtr)
	{
		*((void**)ValueAddr) = ObjRef->GetThisAddr();
	}
}

void  ReadScriptMapIterator(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef && ObjRef->RefType == EFCObjRefType::MapIterator)
	{
		// 脚本写入UE反射对象？这个不因该执行到这里
	}
}

void  ReadScriptWeakObject(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FWeakObjectPtr *WeakPtr = (FWeakObjectPtr *)ValueAddr;

	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if(ObjRef)
	{
		if (FCPropertyType::FCPROPERTY_WeakObjectPtr == ObjRef->DynamicProperty->Type)
		{
			FWeakObjectPtr* ScriptPtr = (FWeakObjectPtr*)ObjRef->GetPropertyAddr();
			*WeakPtr = ScriptPtr->Get();
		}
		else if (FCPROPERTY_ObjectProperty == ObjRef->DynamicProperty->Type)
		{
			*WeakPtr = ObjRef->GetUObject();
		}
	}
}
void  ReadScriptLazyObject(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    FLazyObjectPtr* LazyPtr = (FLazyObjectPtr*)ValueAddr;
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef)
	{
		if (FCPropertyType::FCPROPERTY_LazyObjectPtr == ObjRef->DynamicProperty->Type)
		{
			FLazyObjectPtr* ScriptPtr = (FLazyObjectPtr*)ObjRef->GetPropertyAddr();
			*LazyPtr = ScriptPtr->Get();
		}
		else if (FCPROPERTY_ObjectProperty == ObjRef->DynamicProperty->Type)
		{
			*LazyPtr = ObjRef->GetUObject();
		}
	}
}
void  ReadScriptTArray(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef)
	{
		if(EFCObjRefType::NewTArray == ObjRef->RefType)
		{
			FArrayProperty* Property = (FArrayProperty*)DynamicProperty->Property;
            if (DynamicProperty->bTempNeedRef)
            {
                ((FCDynamicPropertyBase*)DynamicProperty)->bTempRealRef = true;
                FMemory::Memcpy(ValueAddr, ObjRef->GetPropertyAddr(), sizeof(FScriptArray));
            }
            else
            {
                Property->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->Property->ArrayDim);
            }
		}
		else if(DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FArrayProperty* Property = (FArrayProperty*)DynamicProperty->Property;
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
			{
                Property->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->Property->ArrayDim);
			}
		}
	}
}

void ReadScriptTMap(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef)
    {
        if (EFCObjRefType::NewTMap == ObjRef->RefType)
        {
            FMapProperty* Property = (FMapProperty*)DynamicProperty->Property;
            //Property->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->Property->ArrayDim);
            if(DynamicProperty->bTempNeedRef)
            {
                ((FCDynamicPropertyBase *)DynamicProperty)->bTempRealRef = true;
                FMemory::Memcpy(ValueAddr, ObjRef->GetPropertyAddr(), sizeof(FScriptMap));
            }
            else
            {
                Property->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Map)
        {
            FMapProperty* Property = (FMapProperty*)DynamicProperty->Property;
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                //Property->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->Property->ArrayDim);
                Property->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
    }
}

void ReadScriptTSet(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef)
    {
        if (EFCObjRefType::NewTSet == ObjRef->RefType)
        {
            FSetProperty* Property = (FSetProperty*)DynamicProperty->Property;
            //Property->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->Property->ArrayDim);
            if (DynamicProperty->bTempNeedRef)
            {
                ((FCDynamicPropertyBase*)DynamicProperty)->bTempRealRef = true;
                FMemory::Memcpy(ValueAddr, ObjRef->GetPropertyAddr(), sizeof(FScriptSet));
            }
            else
            {
                Property->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
        {
            FSetProperty* Property = (FSetProperty*)DynamicProperty->Property;
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                //Property->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->Property->ArrayDim);
                Property->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
    }
}

void  InitDynamicPropertyReadFunc(FCDynamicProperty *DynamicProperty, FCPropertyType Flag)
{
	if(DynamicProperty->m_ReadScriptFunc)
	{
		return ;
	}
	DynamicProperty->m_ReadScriptFunc = ReadScriptDefault;
	switch(Flag)
	{
		case FCPROPERTY_BoolProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptBool;
			break;
		case FCPROPERTY_Int8Property:
		case FCPROPERTY_ByteProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptInt8;
			break;
		case FCPROPERTY_Int16Property:
			DynamicProperty->m_ReadScriptFunc = ReadScriptInt16;
			break;
		case FCPROPERTY_IntProperty:
		case FCPROPERTY_UInt32Property:
			DynamicProperty->m_ReadScriptFunc = ReadScriptInt32;
			break;
		case FCPROPERTY_FloatProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptFloat;
			break;
		case FCPROPERTY_DoubleProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptDouble;
			break;
		case FCPROPERTY_Int64Property:
		case FCPROPERTY_UInt64Property:
			DynamicProperty->m_ReadScriptFunc = ReadScriptInt64;
			break;
		case FCPROPERTY_NameProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptFName;
			break;
		case FCPROPERTY_StrProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptFString;
			break;
		case FCPROPERTY_TextProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptFText;
			break;
		case FCPROPERTY_ObjectProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptUObject;
			break;
		case FCPROPERTY_WeakObjectPtr:
			DynamicProperty->m_ReadScriptFunc = ReadScriptWeakObject;
			break;
		case FCPROPERTY_LazyObjectPtr:
			DynamicProperty->m_ReadScriptFunc = ReadScriptLazyObject;
			break;
		case FCPROPERTY_StructProperty:
			DynamicProperty->m_ReadScriptFunc = ReadScriptStruct;
			break;
        case FCPROPERTY_Vector2:
            DynamicProperty->m_ReadScriptFunc = ReadScriptStruct;
			break;
        case FCPROPERTY_Vector3:
            DynamicProperty->m_ReadScriptFunc = ReadScriptStruct;
			break;
        case FCPROPERTY_Vector4:
            DynamicProperty->m_ReadScriptFunc = ReadScriptStruct;
			break;
		case FCPROPERTY_Array:
			DynamicProperty->m_ReadScriptFunc = ReadScriptTArray;
			break;
        case FCPROPERTY_Map:
            DynamicProperty->m_ReadScriptFunc = ReadScriptTMap;
            break;
        case FCPROPERTY_Set:
            DynamicProperty->m_ReadScriptFunc = ReadScriptTSet;
            break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
int  FCInnerCallScriptFunc(FCScriptContext* Context, UObject *Object, int64 ScriptIns, FCDynamicFunction* DynamicFunction, FFrame& TheStack, int (*LPPreparePushCallback)(FCScriptContext *, const void *), const void *UserData)
{
	lua_State *L = Context->m_LuaState;
	if(!L)
	{
		return 0;
	}
	int32 MessageHandlerIdx = lua_gettop(L) - 1;

	int nParamCount = DynamicFunction->ParamCount;
	const FCDynamicProperty *BeginProperty = DynamicFunction->m_Property.data();
	const FCDynamicProperty *EndProperty = BeginProperty + nParamCount;
	const FCDynamicProperty *DynamicProperty = BeginProperty;

	uint8  *Locals = TheStack.Locals;
	uint8  *ValueAddr = Locals;
	if (ScriptIns)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, ScriptIns);
		++nParamCount;
	}
	// 输入脚本参数
	if (LPPreparePushCallback)
	{
		nParamCount += LPPreparePushCallback(Context, UserData);
	}
	int Index = 0;
	for (; DynamicProperty < EndProperty; ++DynamicProperty, ++Index)
	{
		ValueAddr = Locals + DynamicProperty->Offset_Internal;
		DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
	}

    // 执行脚本代码
	int32 Code = lua_pcall(L, nParamCount, LUA_MULTRET, MessageHandlerIdx);
	if (Code != LUA_OK)
	{
		ReportLuaCallError(L);
	}
    int32 TopIdx = lua_gettop(L);

	// 拷贝返回值
	int RetCount = 0;
	if( DynamicFunction->ReturnPropertyIndex >= 0)
	{
		const FCDynamicProperty *ReturnProperty = BeginProperty + DynamicFunction->ReturnPropertyIndex;
		ValueAddr = Locals + ReturnProperty->Offset_Internal;
		ReturnProperty->m_WriteScriptFunc(L, ReturnProperty, ValueAddr, nullptr, nullptr);
		++RetCount;
	}

	// 如果有返回值的话(这里脚本层限制上栈变量的访问范围，不可越过当前函数的设置)
	if (DynamicFunction->bOuter)
	{
		DynamicProperty = BeginProperty;
		Index = 0;
		for (; DynamicProperty < EndProperty; ++DynamicProperty, ++Index)
		{
			if (DynamicProperty->bOuter)
			{
				ValueAddr = Locals + DynamicProperty->Offset_Internal;
				DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
				++RetCount;
			}
		}
	}
	return RetCount;
}
bool  FCCallScriptFunc(FCScriptContext* Context, UObject *Object, int64 ScriptIns, const char *ScriptFuncName, FCDynamicFunction* DynamicFunction, FFrame& TheStack)
{
    lua_State* L = Context->m_LuaState;
    if (!L)
    {
        return false;
    }
	int StartIdx = lua_gettop(L);
	if (ScriptIns > 0)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, ScriptIns);
		lua_pushstring(L, ScriptFuncName);
		lua_gettable(L, -2);
	}
	else
	{
		lua_getglobal(L, ScriptFuncName);
	}
	if(lua_isfunction(L, -1))
		FCInnerCallScriptFunc(Context, Object, ScriptIns, DynamicFunction, TheStack, nullptr, nullptr);
	else
	{
		UE_LOG(LogFCScript, Error, TEXT("Invalid Script call, ScriptIns = %d : %s"), ScriptIns, UTF8_TO_TCHAR(ScriptFuncName));
	}
	int CurIdx = lua_gettop(L);
	if (CurIdx > StartIdx)
	{
		lua_pop(L, CurIdx - StartIdx);
	}
    return true;
}

int ScriptDelegatePrepareCall(FCScriptContext * Context, const void* UserData)
{
	lua_State* L = Context->m_LuaState;
	const FCDelegateInfo* DelegateInfo = (const FCDelegateInfo*)UserData;
	for (int i = 0; i < DelegateInfo->ParamCount; ++i)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, DelegateInfo->CallbackParams[i]);
	}
	return DelegateInfo->ParamCount;
}

void  FCCallScriptDelegate(FCScriptContext *Context, UObject *Object, int64 ScriptIns, const FCDelegateInfo& DelegateInfo, FCDynamicFunction* DynamicFunction, FFrame& TheStack)
{
    lua_State* L = Context->m_LuaState;
	if(L)
    {
		int StartIdx = lua_gettop(L);
		lua_rawgeti(L, LUA_REGISTRYINDEX, DelegateInfo.FunctionRef);
		if(lua_isfunction(L, -1))
			FCInnerCallScriptFunc(Context, Object, ScriptIns, DynamicFunction, TheStack, ScriptDelegatePrepareCall, &DelegateInfo);
		else
		{
			UE_LOG(LogFCScript, Error, TEXT("Invalid Script call, FunctionRef = %d : %s"), DelegateInfo.FunctionRef, UTF8_TO_TCHAR(DynamicFunction->Name.c_str()));
		}
		int CurIdx = lua_gettop(L);
		if (CurIdx > StartIdx)
		{
			lua_pop(L, CurIdx - StartIdx);
		}
	}
}