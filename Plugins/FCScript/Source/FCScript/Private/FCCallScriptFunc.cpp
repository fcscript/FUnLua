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
#include "FCTemplateType.h"
#include "FCDynamicDelegateManager.h"

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
    PushScriptStruct(L, DynamicProperty, ValueAddr, ThisObj, ObjRefPtr);
}
void  PushScriptFVector2D(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    PushScriptStruct(L, DynamicProperty, ValueAddr, ThisObj, ObjRefPtr);
}
void  PushScriptFVector4D(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    PushScriptStruct(L, DynamicProperty, ValueAddr, ThisObj, ObjRefPtr);
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

    UStruct* PropertyStruct = DynamicProperty->SafePropertyPtr->GetOwnerStruct();
	if( ClassDesc->m_Struct != PropertyStruct)
	{
		// 必须是子类(这个地方如果会慢的话，可以用hash_map优化)
		if(PropertyStruct->IsChildOf(ClassDesc->m_Struct))
		{
			return true;
		}

		const char* Name = GetUEClassName(ClassDesc->m_UEClassName);
        if (DynamicProperty->Name == Name || strcmp(DynamicProperty->Name, Name) == 0)
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
	// lua 里面没有强类型，所以这里就直接写入吧
	{
		int64 ObjID = 0;
		{
            if(DynamicProperty->bTempNeedRef)
            {
                ObjID = FCGetObj::GetIns()->PushTempRefProperty((const FCDynamicProperty*)DynamicProperty, ValueAddr);
            }
			else if (ThisObj)
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

void PushScriptEnum(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCScript::SetArgValue(L, *ValueAddr);
}

// 将UE对象写入脚本对象
void  PushClassProperty(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    UObject* Object = *((UObject**)ValueAddr);
    FCScript::PushUObject(L, Object);
}

void  PushScriptUObject(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	UObject* value = *((UObject**)ValueAddr);
	FCScript::PushUObject(L, value);
}

void PushScriptObjectPtr(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    // 直接这样也行，更高效，与下面的功能是一样的
    UObject* value = *((UObject**)ValueAddr);
    FCScript::PushUObject(L, value);

    //FObjectPtr  *value = (FObjectPtr*)ValueAddr;
    //FCScript::PushUObject(L, value->Get());
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
        if(WeakPtr)
        {
            FWeakObjectPtr* ScriptPt = new FWeakObjectPtr(*WeakPtr);
            FCDynamicProperty* InDynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_WeakObjectPtr, "FWeakObjectPtr", sizeof(FWeakObjectPtr));
            ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)InDynamicProperty, ScriptPt, EFCObjRefType::NewTWeakPtr);
            FCScript::PushBindObjRef(L, ObjID, "FWeakObjectPtr");
            return ;
        }
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
        if (LazyPtr)
        {
            FLazyObjectPtr* ScriptPt = new FLazyObjectPtr(*LazyPtr);
            FCDynamicProperty* InDynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_LazyObjectPtr, "TLazyObjectPtr", sizeof(FLazyObjectPtr));
            ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)InDynamicProperty, ScriptPt, EFCObjRefType::NewTLazyPtr);
            FCScript::PushBindObjRef(L, ObjID, "TLazyObjectPtr");
            return ;
        }
	}
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushSoftObjectPtr(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSoftObjectPtr* InSoftPtr = (FSoftObjectPtr*)ValueAddr;
    int64 ObjID = 0;
    if (ThisObj)
    {
        ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else
    {
        if(InSoftPtr)
        {
            FCDynamicProperty* InDynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_SoftObjectReference, "TSoftObjectPtr", sizeof(FSoftObjectPtr));
            FSoftObjectPtr* SoftObjectPtr = new FSoftObjectPtr(*InSoftPtr);
            ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)InDynamicProperty, SoftObjectPtr, EFCObjRefType::NewTSoftObjectPtr);
            FCScript::PushBindObjRef(L, ObjID, InDynamicProperty->GetClassName());
            return;
        }
    }
    FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushSoftClassPtr(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSoftObjectPtr* InSoftPtr = (FSoftObjectPtr*)ValueAddr;
    int64 ObjID = 0;
    if (ThisObj)
    {
        ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else
    {
        if(InSoftPtr)
        {
            FCDynamicProperty* InDynamicProperty = GetDynamicPropertyByCppType(FCPROPERTY_SoftClassReference, "TSoftClassPtr", sizeof(FSoftObjectPtr));
            FSoftObjectPtr* SoftObjectPtr = new FSoftObjectPtr(*InSoftPtr);
            ObjID = FCGetObj::GetIns()->PushTemplate((const FCDynamicProperty*)InDynamicProperty, SoftObjectPtr, EFCObjRefType::NewTSoftClassPtr);
            FCScript::PushBindObjRef(L, ObjID, InDynamicProperty->GetClassName());
            return;
        }
    }
    FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptDelegate(lua_State* L, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
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

void PushScriptSpareDelegate(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    return PushScriptDelegate(L, DynamicProperty, ValueAddr, ThisObj, ObjRefPtr);
}

void  PushScriptTArray(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	//FArrayProperty* ArrayProperty = DynamicProperty->SafePropertyPtr->CastArrayProperty();
	int64 ObjID = 0;
	// 如果变量是UObject的属性
    if(DynamicProperty->bTempNeedRef)
    {
        ObjID = FCGetObj::GetIns()->PushTempRefProperty((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
	else if (ThisObj)
	{
		ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	else
	{
        if(ObjRefPtr)
            ObjID = FCGetObj::GetIns()->PushChildProperty((FCObjRef *)ObjRefPtr, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
        else
            ObjID = FCGetObj::GetIns()->PushNewTArray((const FCDynamicProperty*)DynamicProperty, ValueAddr);
	}
	FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

void  PushScriptTMap(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FMapProperty* MapProperty = DynamicProperty->SafePropertyPtr->CastMapProperty();
    int64 ObjID = 0;
    // 如果变量是UObject的属性
    if (DynamicProperty->bTempNeedRef)
    {
        ObjID = FCGetObj::GetIns()->PushTempRefProperty((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else if (ThisObj)
    {
        ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else
    {
        if (ObjRefPtr)
            ObjID = FCGetObj::GetIns()->PushChildProperty((FCObjRef*)ObjRefPtr, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
        else
            ObjID = FCGetObj::GetIns()->PushNewTMap((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());    
}

void  PushScriptTSet(lua_State* L, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    //FSetProperty* MapProperty = DynamicProperty->SafePropertyPtr->CastSetProperty();
    int64 ObjID = 0;
    // 如果变量是UObject的属性
    if (DynamicProperty->bTempNeedRef)
    {
        ObjID = FCGetObj::GetIns()->PushTempRefProperty((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else if (ThisObj)
    {
        ObjID = FCGetObj::GetIns()->PushProperty(ThisObj, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    else
    {
        if (ObjRefPtr)
            ObjID = FCGetObj::GetIns()->PushChildProperty((FCObjRef*)ObjRefPtr, (const FCDynamicProperty*)DynamicProperty, ValueAddr);
        else
            ObjID = FCGetObj::GetIns()->PushNewTSet((const FCDynamicProperty*)DynamicProperty, ValueAddr);
    }
    FCScript::PushBindObjRef(L, ObjID, DynamicProperty->GetClassName());
}

LPPushScriptValueFunc  InitDynamicPropertyWriteFunc(FCPropertyType Flag)
{
    LPPushScriptValueFunc WriteFunc = PushScriptDefault;
	switch(Flag)
	{
		case FCPROPERTY_BoolProperty:
            WriteFunc = PushScriptBool;
			break;
		case FCPROPERTY_Int8Property:
		case FCPROPERTY_ByteProperty:
            WriteFunc = PushScriptInt8;
			break;
		case FCPROPERTY_Int16Property:
            WriteFunc = PushScriptInt16;
			break;
		case FCPROPERTY_IntProperty:
		case FCPROPERTY_UInt32Property:
            WriteFunc = PushScriptInt32;
			break;
		case FCPROPERTY_FloatProperty:
            WriteFunc = PushScriptFloat;
			break;
		case FCPROPERTY_DoubleProperty:
            WriteFunc = PushScriptDouble;
			break;
		case FCPROPERTY_Int64Property:
		case FCPROPERTY_UInt64Property:
            WriteFunc = PushScriptInt64;
			break;
		case FCPROPERTY_NameProperty:
            WriteFunc = PushScriptFName;
			break;
		case FCPROPERTY_StrProperty:
            WriteFunc = PushScriptFString;
			break;
		case FCPROPERTY_TextProperty:
            WriteFunc = PushScriptFText;
			break;
		case FCPROPERTY_ObjectProperty:
            WriteFunc = PushScriptUObject;
			break;
        case FCPROPERTY_ObjectPtrProperty:
            WriteFunc = PushScriptObjectPtr;
            break;
		case FCPROPERTY_WeakObjectPtr:
            WriteFunc = PushScriptWeakObject;
			break;
		case FCPROPERTY_LazyObjectPtr:
            WriteFunc = PushScriptLazyObject;
			break;
        case FCPROPERTY_SoftObjectReference:
            WriteFunc = PushSoftObjectPtr;
            break;
        case FCPROPERTY_SoftClassReference:
            WriteFunc = PushSoftClassPtr;
            break;
		case FCPROPERTY_StructProperty:
            WriteFunc = PushScriptStruct;
			break;
		case FCPROPERTY_Vector2:
            WriteFunc = PushScriptStruct;
			break;
		case FCPROPERTY_Vector3:
            WriteFunc = PushScriptStruct;
			break;
		case FCPROPERTY_Vector4:
            WriteFunc = PushScriptStruct;
			break;
        case FCPROPERTY_Enum:
            WriteFunc = PushScriptEnum;
            break;
        case FCPROPERTY_ClassProperty:
            WriteFunc = PushClassProperty;
            break;
		case FCPROPERTY_Array:
            WriteFunc = PushScriptTArray;
			break;
        case FCPROPERTY_Map:
            WriteFunc = PushScriptTMap;
            break;
        case FCPROPERTY_Set:
            WriteFunc  = PushScriptTSet;
            break;
		case FCPROPERTY_DelegateProperty:
		case FCPROPERTY_MulticastDelegateProperty:
            WriteFunc = PushScriptDelegate;
			break;
        case FCPROPERTY_MulticastSparseDelegateProperty:
            WriteFunc = PushScriptSpareDelegate;
            break;
		default:
			break;
	}
    return WriteFunc;
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
void  ReadScriptFVector2D(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_Vector2)
    {
        *((FVector2D*)ValueAddr) = *((const FVector2D*)ObjRef->GetPropertyAddr());
    }
}
void  ReadScriptFVector(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_Vector3)
    {
        *((FVector*)ValueAddr) = *((const FVector*)ObjRef->GetPropertyAddr());
    }
}
void  ReadScriptFVector4D(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_Vector4)
    {
        *((FVector4*)ValueAddr) = *((const FVector4*)ObjRef->GetPropertyAddr());
    }
}
void ReadScriptEnum(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    int Value = lua_tointeger(L, ValueIdx);
    *ValueAddr = (uint8)Value;
}
// 将脚本对象写入到UE对象
void  ReadScriptStruct(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	FCObjRef* ObjRef = (FCObjRef *)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef && ObjRef->ClassDesc && ObjRef->ClassDesc->m_Struct == DynamicProperty->SafePropertyPtr->GetPropertyStruct())
	{
        DynamicProperty->SafePropertyPtr->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->SafePropertyPtr->ArrayDim);
	}
    else
    {
        ReportLuaError(L, "invalid struct param, none copy");
    }
}

void  ReadClassProperty(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    UClass *MetaClass = DynamicProperty->SafePropertyPtr->GetPropertyMetaClass();
    UObject *Object = FCScript::GetUStructOrUObject(L, ValueIdx);
    UClass* InClass = Object ? Object->GetClass() : nullptr;
    if(InClass)
    {
        if(MetaClass == InClass || InClass->IsChildOf(InClass))
        {
            *((UObject**)ValueAddr) = Object;
        }
        else
        {
            *((UObject**)ValueAddr) = nullptr;
        }
    }
    else
    {
        *((UObject**)ValueAddr) = nullptr;
    }
}

void  ReadScriptUObject(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
	UObject *SrcObj = FCScript::GetUObject(L, ValueIdx);
    UClass* InClass = Cast<UClass>(SrcObj);
    if(!InClass)
    {
        InClass = SrcObj ? SrcObj->GetClass() : UObject::StaticClass();
    }
    UClass* Class = DynamicProperty->SafePropertyPtr->GetPropertyClass();
    if (Class == InClass || InClass->IsChildOf(Class))
    {
        *((UObject**)ValueAddr) = SrcObj;
    }
    else
    {
        *((UObject**)ValueAddr) = nullptr;
        if(SrcObj)
        {
            ReportLuaError(L, "invalid object param, cast to nullptr");
        }
    }
}

void  ReadScriptObjectPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FObjectPtr  *DesValue = (FObjectPtr *)ValueAddr;
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if(DynamicProperty->Type == FCPropertyType::FCPROPERTY_ObjectPtrProperty)
    {
        if(ObjRef && ObjRef->IsValid())
        {
            *DesValue = *((FObjectPtr*)ObjRef->GetPropertyAddr());
        }
        else
        {
            *DesValue = nullptr;
        }
    }
    else if(ObjRef && ObjRef->IsValid())
    {
        *DesValue = ObjRef->GetUObject();
    }
    else
    {
        *DesValue = nullptr;
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
        UObject *InObject = ObjRef->GetUObject(); 
        if(InObject)
        {
            *WeakPtr = InObject;
        }
        else if(ObjRef->DynamicProperty)
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
}
void  ReadScriptLazyObject(lua_State* L, int ValueIdx, const FCDynamicPropertyBase *DynamicProperty, uint8  *ValueAddr, UObject *ThisObj, void* ObjRefPtr)
{
    FLazyObjectPtr* LazyPtr = (FLazyObjectPtr*)ValueAddr;
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef)
	{
        UObject* InObject = ObjRef->GetUObject();
        if (InObject)
        {
            *LazyPtr = InObject;
        }
        else if (ObjRef->DynamicProperty)
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
}
void  ReadScriptSoftObjectPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSoftObjectPtr* SoftPtr = (FSoftObjectPtr*)ValueAddr;
    int Type = lua_type(L, ValueIdx);
    if (LUA_TSTRING == Type)
    {
        // 如果是字符串，就表示是路径
        const char *ArgPath = lua_tostring(L, ValueIdx);
        FString  AssetPath(UTF8_TO_TCHAR(ArgPath));
        FSoftObjectPath  ObjPath(AssetPath);
        *SoftPtr = FSoftObjectPtr(ObjPath);
    }
    else
    {
        FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
        if (ObjRef)
        {
            UObject* InObject = ObjRef->GetUObject();
            if (InObject)
            {
                *SoftPtr = InObject;
            }
            else if(ObjRef->DynamicProperty)
            {
                if (FCPropertyType::FCPROPERTY_SoftObjectReference == ObjRef->DynamicProperty->Type)
                {
                    FSoftObjectPtr* ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
                    *SoftPtr = *ScriptPtr;
                }
                else if (FCPROPERTY_ObjectProperty == ObjRef->DynamicProperty->Type)
                {
                    *SoftPtr = ObjRef->GetUObject();
                }
            }
        }        
    }
}

void ReadScriptSoftClassPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSoftObjectPtr* SoftPtr = (FSoftObjectPtr*)ValueAddr;
    int Type = lua_type(L, ValueIdx);
    if (LUA_TSTRING == Type)
    {
        // 如果是字符串，就表示是路径
        const char* ArgPath = lua_tostring(L, ValueIdx);
        FString  AssetPath(UTF8_TO_TCHAR(ArgPath));
        FSoftObjectPath  ObjPath(AssetPath);
        *SoftPtr = FSoftObjectPtr(ObjPath);
    }
    else
    {
        if (LUA_TTABLE == Type)
        {
            UStruct *Struct = FCScript::GetUStruct(L, ValueIdx);
            UClass* InClass = Cast<UClass>(Struct);
            if(InClass)
            {
                *SoftPtr = InClass;
                return ;
            }
        }
        FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
        if (ObjRef)
        {
            UObject* InObject = ObjRef->GetUObject();
            if (InObject)
            {
                UClass* InClass = Cast<UClass>(InObject);
                *SoftPtr = InClass;
            }
            else if(ObjRef->DynamicProperty)
            {
                if (FCPropertyType::FCPROPERTY_SoftClassReference == ObjRef->DynamicProperty->Type)
                {
                    FSoftObjectPtr* ScriptPtr = (FSoftObjectPtr*)ObjRef->GetPropertyAddr();
                    *SoftPtr = *ScriptPtr;
                    return;
                }
                else if (FCPROPERTY_ObjectProperty == ObjRef->DynamicProperty->Type)
                {
                    UClass* InClass = Cast<UClass>(ObjRef->GetUObject());
                    *SoftPtr = InClass;
                    return;
                }
                else if (FCPROPERTY_ClassProperty == ObjRef->DynamicProperty->Type)
                {
                    UClass* InClass = *((UClass**)ObjRef->GetPropertyAddr());
                    *SoftPtr = InClass;
                    return;
                }
            }
        }
        *SoftPtr = nullptr;
    }
}

void  ReadScriptTArray(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
	if (ObjRef)
	{
		if(EFCObjRefType::NewTArray == ObjRef->RefType)
		{
			FArrayProperty* Property = DynamicProperty->SafePropertyPtr->CastArrayProperty();
            if (DynamicProperty->bTempNeedRef)
            {
                ((FCDynamicPropertyBase*)DynamicProperty)->bTempRealRef = true;
                FMemory::Memcpy(ValueAddr, ObjRef->GetPropertyAddr(), sizeof(FScriptArray));
            }
            else
            {
                DynamicProperty->SafePropertyPtr->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->SafePropertyPtr->ArrayDim);
            }
		}
		else if(DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FArrayProperty* Property = DynamicProperty->SafePropertyPtr->CastArrayProperty();
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
			{
                DynamicProperty->SafePropertyPtr->CopyValuesInternal(ValueAddr, ObjRef->GetPropertyAddr(), DynamicProperty->SafePropertyPtr->ArrayDim);
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
            FMapProperty* Property = DynamicProperty->SafePropertyPtr->CastMapProperty();
            if(DynamicProperty->bTempNeedRef)
            {
                ((FCDynamicPropertyBase *)DynamicProperty)->bTempRealRef = true;
                FMemory::Memcpy(ValueAddr, ObjRef->GetPropertyAddr(), sizeof(FScriptMap));
            }
            else
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Map)
        {
            FMapProperty* Property = DynamicProperty->SafePropertyPtr->CastMapProperty();
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
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
            FSetProperty* Property = DynamicProperty->SafePropertyPtr->CastSetProperty();
            if (DynamicProperty->bTempNeedRef)
            {
                ((FCDynamicPropertyBase*)DynamicProperty)->bTempRealRef = true;
                FMemory::Memcpy(ValueAddr, ObjRef->GetPropertyAddr(), sizeof(FScriptSet));
            }
            else
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
        {
            FSetProperty* Property = DynamicProperty->SafePropertyPtr->CastSetProperty();
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ValueAddr, ObjRef->GetPropertyAddr());
            }
        }
    }
}

void ReadScriptDelegate(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
// 这个还是不要支持了，因为不好追踪生命周期, 只能传入UObject，按该绑定的对象的生命周期来
    FScriptDelegate* Delegate = (FScriptDelegate*)ValueAddr;
    int Type = lua_type(L, ValueIdx);
    FCLuaDelegate* LuaDelegate = nullptr;
    if (LUA_TFUNCTION == Type)
    {
        const void *FuncAddr = lua_topointer(L, ValueIdx);  // 单纯的绑定一个函数，无法确认名字，但可以用函数地址做函数名, 如果该对象释放，则这个函数也释放
        LuaDelegate = FCDynamicDelegateManager::GetIns().MakeLuaDelegate(nullptr, nullptr, L, ValueIdx, DynamicProperty);
    }
    else if(LUA_TTABLE == Type)
    {
         // { self, func } 这种形式
        LuaDelegate = FCDynamicDelegateManager::GetIns().MakeDelegateByTableParam(L, ValueIdx, DynamicProperty);
    }
    else
    {
        FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
        if(ObjRef && ObjRef->RefType == EFCObjRefType::LuaDelegate)
        {
            FCDelegateInfo* DelegateInfo = (FCDelegateInfo*)ObjRef->GetPropertyAddr();
            LuaDelegate = FCDynamicDelegateManager::GetIns().MakeDelegate(L, DelegateInfo, DynamicProperty);
        }
    }
    if (LuaDelegate)
        *Delegate = LuaDelegate->Delegate;
}

void ReadScriptMulticastDelegate(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    int iii = 0;    
}
void ReadScriptMulticastSparseDelegate(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    int iiii = 0;
}

LPOuterScriptValueFunc  InitDynamicPropertyReadFunc(FCPropertyType Flag)
{
    LPOuterScriptValueFunc  ReadScriptFunc = ReadScriptDefault;
	switch(Flag)
	{
		case FCPROPERTY_BoolProperty:
            ReadScriptFunc = ReadScriptBool;
			break;
		case FCPROPERTY_Int8Property:
		case FCPROPERTY_ByteProperty:
            ReadScriptFunc = ReadScriptInt8;
			break;
		case FCPROPERTY_Int16Property:
            ReadScriptFunc = ReadScriptInt16;
			break;
		case FCPROPERTY_IntProperty:
		case FCPROPERTY_UInt32Property:
            ReadScriptFunc = ReadScriptInt32;
			break;
		case FCPROPERTY_FloatProperty:
            ReadScriptFunc = ReadScriptFloat;
			break;
		case FCPROPERTY_DoubleProperty:
            ReadScriptFunc = ReadScriptDouble;
			break;
		case FCPROPERTY_Int64Property:
		case FCPROPERTY_UInt64Property:
            ReadScriptFunc = ReadScriptInt64;
			break;
		case FCPROPERTY_NameProperty:
            ReadScriptFunc = ReadScriptFName;
			break;
		case FCPROPERTY_StrProperty:
            ReadScriptFunc = ReadScriptFString;
			break;
		case FCPROPERTY_TextProperty:
            ReadScriptFunc = ReadScriptFText;
			break;
		case FCPROPERTY_ObjectProperty:
            ReadScriptFunc = ReadScriptUObject;
			break;
        case FCPROPERTY_ObjectPtrProperty:
            ReadScriptFunc = ReadScriptObjectPtr;
            break;
		case FCPROPERTY_WeakObjectPtr:
            ReadScriptFunc = ReadScriptWeakObject;
			break;
		case FCPROPERTY_LazyObjectPtr:
            ReadScriptFunc = ReadScriptLazyObject;
			break;
        case FCPROPERTY_SoftObjectReference:
            ReadScriptFunc = ReadScriptSoftObjectPtr;
            break;
        case FCPROPERTY_SoftClassReference:
            ReadScriptFunc = ReadScriptSoftClassPtr;
            break;
		case FCPROPERTY_StructProperty:
            ReadScriptFunc = ReadScriptStruct;
			break;
        case FCPROPERTY_Vector2:
            ReadScriptFunc = ReadScriptFVector2D;
            break;
        case FCPROPERTY_Vector3:
            ReadScriptFunc = ReadScriptFVector;
            break;
        case FCPROPERTY_Vector4:
            ReadScriptFunc = ReadScriptFVector4D;
            break;
        case FCPROPERTY_Enum:
            ReadScriptFunc = ReadScriptEnum;
            break;
        case FCPROPERTY_ClassProperty:
            ReadScriptFunc = ReadClassProperty;
            break;
		case FCPROPERTY_Array:
            ReadScriptFunc = ReadScriptTArray;
			break;
        case FCPROPERTY_Map:
            ReadScriptFunc = ReadScriptTMap;
            break;
        case FCPROPERTY_Set:
            ReadScriptFunc = ReadScriptTSet;
            break;
        case FCPROPERTY_DelegateProperty:
            ReadScriptFunc = ReadScriptDelegate;
            break;
        case FCPROPERTY_MulticastDelegateProperty:
            ReadScriptFunc = ReadScriptMulticastDelegate;
            break;
        case FCPROPERTY_MulticastSparseDelegateProperty:
            ReadScriptFunc = ReadScriptMulticastSparseDelegate;
            break;
		default:
			break;
	}
    return ReadScriptFunc;
}

//---------------------------------------------------------------------------------

bool  CopyScriptDefault(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    return false;
}

// 将已经存在上的栈变量，添加到lua
// 说明：直接将lua栈上的变量再push回去，不能增加C++端对象的引用计数,因为lua虚拟机不会增加引用计数
void PushLuaExitValue(lua_State* L, int ValueIdx, FCObjRef* ObjRef)
{
    lua_pushvalue(L, ValueIdx);
}

// 将脚本对象写入到UE对象
bool  CopyScriptStruct(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->ClassDesc && ObjRef->ClassDesc->m_Struct == DynamicProperty->SafePropertyPtr->GetPropertyStruct())
    {
        DynamicProperty->SafePropertyPtr->CopyValuesInternal(ObjRef->GetPropertyAddr(), ValueAddr, DynamicProperty->SafePropertyPtr->ArrayDim);
        PushLuaExitValue(L, ValueIdx, ObjRef);
        return true;
    }
    else
    {
        ReportLuaError(L, "invalid struct param, none copy");
    }
    return false;
}

// 将脚本对象写入到UE对象
bool  CopyScriptFVector2D(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_Vector2)
    {
        *((FVector2D*)ObjRef->GetPropertyAddr()) = *((const FVector2D*)ValueAddr);
        PushLuaExitValue(L, ValueIdx, ObjRef);
        return true;
    }
    return false;
}
bool  CopyScriptFVector(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_Vector3)
    {
        *((FVector*)ObjRef->GetPropertyAddr()) = *((const FVector*)ValueAddr);
        PushLuaExitValue(L, ValueIdx, ObjRef);
        return true;
    }
    return false;
}
bool  CopyScriptFVector4D(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_Vector4)
    {
        *((FVector4*)ObjRef->GetPropertyAddr()) = *((const FVector4*)ValueAddr);
        PushLuaExitValue(L, ValueIdx, ObjRef);
        return true;
    }
    return false;
}
bool  CopyScriptSoftObjectPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSoftObjectPtr* SoftPtr = (FSoftObjectPtr*)ValueAddr;
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_SoftObjectReference)
    {
        *SoftPtr = *((const FSoftObjectPtr*)ValueAddr);
        PushLuaExitValue(L, ValueIdx, ObjRef);
        return true;
    }
    return false;
}

bool  CopyScriptSoftClassPtr(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FSoftObjectPtr* SoftPtr = (FSoftObjectPtr*)ValueAddr;
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef && ObjRef->GetPropertyType() == FCPropertyType::FCPROPERTY_SoftClassReference)
    {
        *SoftPtr = *((const FSoftObjectPtr*)ValueAddr);
        PushLuaExitValue(L, ValueIdx, ObjRef);
        return true;
    }
    return false;
}

bool  CopyScriptTArray(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    // 将UE对象拷贝到指定lua栈上参数
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef)
    {
        if (EFCObjRefType::NewTArray == ObjRef->RefType)
        {
            if (DynamicProperty->bTempRealRef)
            {
                FMemory::Memcpy(ObjRef->GetPropertyAddr(), ValueAddr, sizeof(FScriptArray));
            }
            else
            {
                DynamicProperty->SafePropertyPtr->CopyValuesInternal(ObjRef->GetPropertyAddr(), ValueAddr, DynamicProperty->SafePropertyPtr->ArrayDim);
            }
            PushLuaExitValue(L, ValueIdx, ObjRef);
            return true;
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
        {
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                DynamicProperty->SafePropertyPtr->CopyValuesInternal(ObjRef->GetPropertyAddr(), ValueAddr, DynamicProperty->SafePropertyPtr->ArrayDim);
                PushLuaExitValue(L, ValueIdx, ObjRef);
                return true;
            }
        }
    }
    return false;
}

bool CopyScriptTMap(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef)
    {
        if (EFCObjRefType::NewTMap == ObjRef->RefType)
        {
            if (DynamicProperty->bTempRealRef)
            {
                FMemory::Memcpy(ObjRef->GetPropertyAddr(), ValueAddr, sizeof(FScriptMap));
            }
            else
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ObjRef->GetPropertyAddr(), ValueAddr);
            }
            PushLuaExitValue(L, ValueIdx, ObjRef);
            return true;
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Map)
        {
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ObjRef->GetPropertyAddr(), ValueAddr);
                PushLuaExitValue(L, ValueIdx, ObjRef);
                return true;
            }
        }
    }
    return false;
}

bool CopyScriptTSet(lua_State* L, int ValueIdx, const FCDynamicPropertyBase* DynamicProperty, uint8* ValueAddr, UObject* ThisObj, void* ObjRefPtr)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, ValueIdx);
    if (ObjRef)
    {
        if (EFCObjRefType::NewTSet == ObjRef->RefType)
        {
            if (DynamicProperty->bTempRealRef)
            {
                FMemory::Memcpy(ObjRef->GetPropertyAddr(), ValueAddr, sizeof(FScriptSet));
            }
            else
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ObjRef->GetPropertyAddr(), ValueAddr);
            }
            PushLuaExitValue(L, ValueIdx, ObjRef);
            return true;
        }
        else if (DynamicProperty->Type == FCPropertyType::FCPROPERTY_Set)
        {
            if (ObjRef->GetPropertyType() == DynamicProperty->Type)
            {
                DynamicProperty->SafePropertyPtr->CopyCompleteValue(ObjRef->GetPropertyAddr(), ValueAddr);
                PushLuaExitValue(L, ValueIdx, ObjRef);
                return true;
            }
        }
    }
    return false;
}

LPCopyScriptValueFunc InitDynamicPropertyCopyFunc(FCPropertyType Flag)
{
    LPCopyScriptValueFunc CopyScriptFunc = CopyScriptDefault;
    switch (Flag)
    {
    case FCPROPERTY_StructProperty:
        CopyScriptFunc = CopyScriptStruct;
        break;
    case FCPROPERTY_Vector2:
        CopyScriptFunc = CopyScriptFVector2D;
        break;
    case FCPROPERTY_Vector3:
        CopyScriptFunc = CopyScriptFVector;
        break;
    case FCPROPERTY_Vector4:
        CopyScriptFunc = CopyScriptFVector4D;
        break;
    case FCPROPERTY_SoftObjectReference:
        CopyScriptFunc = CopyScriptSoftObjectPtr;
        break;
    case FCPROPERTY_SoftClassReference:
        CopyScriptFunc = CopyScriptSoftClassPtr;
        break;
    case FCPROPERTY_Array:
        CopyScriptFunc = CopyScriptTArray;
        break;
    case FCPROPERTY_Map:
        CopyScriptFunc = CopyScriptTMap;
        break;
    case FCPROPERTY_Set:
        CopyScriptFunc = CopyScriptTSet;
        break;
    default:
        break;
    }
    return CopyScriptFunc;
}

//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
int  FCInnerCallScriptFunc(lua_State* L, UObject *Object, int64 ScriptIns, FCDynamicFunction* DynamicFunction, FFrame& TheStack, int (*LPPreparePushCallback)(lua_State*, const void *), const void *UserData)
{
	if(!L)
	{
		return 0;
	}
	int32 MessageHandlerIdx = lua_gettop(L) - 1;

	int nParamCount = DynamicFunction->ParamCount; // DynamicFunction->m_Property.size();
    FCDynamicFunctionParam *BeginProperty = DynamicFunction->m_Property.data();
    FCDynamicFunctionParam *EndProperty = BeginProperty + DynamicFunction->m_Property.size();
    FCDynamicFunctionParam *DynamicProperty = BeginProperty;

    FOutParmRec *OutParms = TheStack.OutParms;

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
		nParamCount += LPPreparePushCallback(L, UserData);
	}

    int TempObjIDCount = FCGetObj::GetIns()->GetTempObjIDCount();
    int ParamStart = lua_gettop(L);
	int Index = 0;
	for (; DynamicProperty < EndProperty; ++DynamicProperty, ++Index)
	{
		ValueAddr = Locals + DynamicProperty->Offset_Internal;
        // 引用对象, 实际上，非&标记的可以使用引用，隐藏规则是lua里面不能对这个变量保存，如果要保存，就需要clone这个变量
        // 主要是针对TArray, TSet, TMap 这些容器，可以大幅度优化性能，然后面后统一删除这个对象
        if(DynamicProperty->bOuter)
        {
            if(OutParms)
            {
                ValueAddr = OutParms->PropAddr;
                if(!DynamicProperty->SafePropertyPtr->HasAnyPropertyFlags(CPF_ConstParm))
                {
                    DynamicProperty->bTempNeedRef = true;
                    DynamicProperty->bTempRealRef = false;
                    DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
                    DynamicProperty->bTempNeedRef = false;
                }
                else
                {
                    DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
                }
                OutParms = OutParms->NextOutParm;
            }
            else
            {
                DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
            }
        }
        else
        {
            DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
        }
	}

    int StartCallIndex = lua_gettop(L);
    // 执行脚本代码
	int32 Code = lua_pcall(L, nParamCount, LUA_MULTRET, MessageHandlerIdx);
	if (Code != LUA_OK)
	{
		//ReportLuaCallError(L);
	}
    int32 TopIdx = lua_gettop(L);

	// 拷贝返回值
	int RetCount = 0;
	if( DynamicFunction->ReturnPropertyIndex >= 0)
	{
        int RetIdx = StartCallIndex - nParamCount; // 因为前面还有一个函数名字, 所以要减掉这个, 取第一个变量
        if(RetIdx <= TopIdx)
        {
            const FCDynamicProperty* ReturnProperty = BeginProperty + DynamicFunction->ReturnPropertyIndex;
            ValueAddr = Locals + ReturnProperty->Offset_Internal;
            if(OutParms)
                ValueAddr = OutParms->PropAddr;
            ReturnProperty->m_ReadScriptFunc(L, RetIdx, DynamicProperty, ValueAddr, nullptr, nullptr);
            ++RetCount;
        }
	}

	// 如果有返回值的话(这里脚本层限制上栈变量的访问范围，不可越过当前函数的设置)
    FCGetObj::GetIns()->ClearTempIDList(TempObjIDCount);

	return RetCount;
}
bool  FCCallScriptFunc(lua_State* L, UObject *Object, int64 ScriptIns, const char *ScriptFuncName, FCDynamicFunction* DynamicFunction, FFrame& TheStack)
{
    if (!L)
    {
        return false;
    }
	int StartIdx = lua_gettop(L);
    lua_pushcfunction(L, ReportLuaCallError);
	if (ScriptIns > 0)
	{
		//lua_rawgeti(L, LUA_REGISTRYINDEX, ScriptIns);
		//lua_pushstring(L, ScriptFuncName);
		//lua_gettable(L, -2);

        RawGetLuaFucntionByScriptIns(L, ScriptIns, ScriptFuncName);
	}
	else
	{
		lua_getglobal(L, ScriptFuncName);
	}
	if(lua_isfunction(L, -1))
		FCInnerCallScriptFunc(L, Object, ScriptIns, DynamicFunction, TheStack, nullptr, nullptr);
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

int ScriptDelegatePrepareCall(lua_State* L, const void* UserData)
{
	const FCDelegateInfo* DelegateInfo = (const FCDelegateInfo*)UserData;
	for (int i = 0; i < DelegateInfo->ParamCount; ++i)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, DelegateInfo->CallbackParams[i]);
	}
	return DelegateInfo->ParamCount;
}

void  FCCallScriptDelegate(lua_State* L, UObject *Object, int64 ScriptIns, const FCDelegateInfo& DelegateInfo, FCDynamicFunction* DynamicFunction, FFrame& TheStack)
{
	if(L)
    {
		int StartIdx = lua_gettop(L);
        lua_pushcfunction(L, ReportLuaCallError);
		lua_rawgeti(L, LUA_REGISTRYINDEX, DelegateInfo.FunctionRef);
		if(lua_isfunction(L, -1))
			FCInnerCallScriptFunc(L, Object, ScriptIns, DynamicFunction, TheStack, ScriptDelegatePrepareCall, &DelegateInfo);
		else
		{
			UE_LOG(LogFCScript, Error, TEXT("Invalid Script call, FunctionRef = %d : %s"), DelegateInfo.FunctionRef, UTF8_TO_TCHAR(DynamicFunction->Name));
		}
		int CurIdx = lua_gettop(L);
		if (CurIdx > StartIdx)
		{
			lua_pop(L, CurIdx - StartIdx);
		}
	}
}