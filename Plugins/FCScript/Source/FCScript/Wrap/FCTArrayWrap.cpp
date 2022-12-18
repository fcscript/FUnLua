#include "FCTArrayWrap.h"
#include "Containers/ScriptArray.h"
#include "FCTemplateType.h"
#include "FCTArrayHelper.h"

#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCRunTimeRegister.h"
#include "../LuaCore/LuaContext.h"

void FCTArrayWrap::Register(lua_State* L)
{
	luaL_requiref(L, "TArray", LibOpen_wrap, 1);
}

int FCTArrayWrap::LibOpen_wrap(lua_State* L)
{
	const LuaRegFunc LibFuncs[] =
	{
		{ "Length", GetNumb_wrap },
		{ "Add", Add_wrap },
		{ "AddUnique", AddUnique_wrap },
		{ "Find", Find_wrap },
		{ "Insert", Insert_wrap },
		{ "Remove", Remove_wrap },
		{ "RemoveItem", RemoveItem_wrap },
		{ "Clear", Clear_wrap },
		{ "Reserve", Reserve_wrap },
		{ "Resize", SetNumb_wrap },
		//{ "GetData", TArray_GetData },
		{ "Get", GetIndex_wrap },
		//{ "GetRef", TArray_GetRef },
		{ "Set", SetIndex_wrap },
		{ "Swap", Swap_wrap },
		//{ "Shuffle", TArray_Shuffle },
		//{ "LastIndex", TArray_LastIndex },
		//{ "IsValidIndex", TArray_IsValidIndex },
		{ "Contains", Contains_wrap },
		{ "Append", Add_wrap },
		{ "ToTable", ToList_wrap },
		{ "__gc", FCExportedClass::obj_Delete },
		{ "__call", obj_new },
		{ "__eq", FCExportedClass::obj_equal },
		//{ "__index", obj_Index },
		//{ "__newindex", obj_NewIndex },
		//{ "__pairs", obj_pairs },
		{ "__len", GetNumb_wrap },
		{ nullptr, nullptr }
	};
	const char* ClassName = lua_tostring(L, 1);
	FCExportedClass::RegisterLibClass(L, ClassName, LibFuncs);
	return 1;
}

int FCTArrayWrap::obj_Index(lua_State* L)
{
	// (table, key)
	int Type = lua_type(L, 2);
	if (Type == LUA_TNUMBER)
	{
		if (lua_isinteger(L, 2))
		{
			return GetIndex_wrap(L);
		}
	}
	else if (Type == LUA_TSTRING)
	{
		const char* key = lua_tostring(L, 2);
		int iii = 0;
	}
	return lua_rawget(L, 1);
}

int FCTArrayWrap::obj_NewIndex(lua_State* L)
{
	int Type = lua_type(L, 2);
	if (Type == LUA_TNUMBER)
	{
		if (lua_isinteger(L, 2))
		{
			return SetIndex_wrap(L);
		}
	}
	lua_rawset(L, 1);
	return 0;
}

int FCTArrayWrap::obj_new(lua_State* L)
{
	// FScriptArray *ScriptArray = new FScriptArray;
	// 这个还是不要让动态构建的好了
	// 因为不管怎么样，就算是相同的，也是需要拷贝的
    const char* PropertyName = GetPropertyType(L, 2);
	FCDynamicProperty *DynamicProperty = GetTArrayDynamicProperty(PropertyName);
	if(DynamicProperty)
	{
		FScriptArray *ScriptArray = new FScriptArray();
		int64 ObjID = FCGetObj::GetIns()->PushTemplate(DynamicProperty, ScriptArray, EFCObjRefType::NewTArray);
		FCScript::PushBindObjRef(L, ObjID, "TArray");
	}
	else
	{
        lua_pushnil(L);
	}
	return 1;
}
int FCTArrayWrap::GetNumb_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if(ObjRef)
	{
		FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
		int Num = ScriptArray->Num();
        lua_pushinteger(L, Num);
	}
	else
	{
        lua_pushinteger(L, 0);
	}
	return 1;
}

void  FCTArrayWrap_SetNumb(FScriptArray *ScriptArray, FProperty *Inner, int NewNum)
{
	if(NewNum < 0)
	{
		NewNum = 0;
	}
	int ElementSize = Inner->GetSize();
	int32 OldNum = ScriptArray->Num();
	uint8 *ObjAddr = (uint8 *)ScriptArray->GetData();
	uint8 *ValueAddr = ObjAddr;
	if (OldNum < NewNum)
	{
		int32 Index = ScriptArray->Add(NewNum - OldNum, ElementSize);
		for (; Index < NewNum; ++Index)
		{
			ValueAddr = ObjAddr + Index * ElementSize;
			Inner->InitializeValue(ValueAddr);
		}
	}
	else
	{
		for (int32 Index = OldNum; Index < NewNum; ++Index)
		{
			ValueAddr = ObjAddr + Index * ElementSize;
			Inner->DestroyValue(ValueAddr);
		}
		ScriptArray->Remove(OldNum, NewNum - OldNum, ElementSize);
	}
}
int FCTArrayWrap::SetNumb_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if(ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			FProperty *Inner = ArrayProperty->Inner;
			int32 NewNum = lua_tointeger(L, 2);
			FCTArrayWrap_SetNumb(ScriptArray, Inner, NewNum);
		}
	}
	return 0;
}
int FCTArrayWrap_GetAt_Wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			int Index = lua_tointeger(L, 2) - 1;  // lua 从 1开始

			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			int32 Num = ScriptArray->Num();
			if(Index >= 0 && Index < Num)
			{
				FProperty *Inner = ArrayProperty->Inner;
				int ElementSize = Inner->GetSize();
				uint8 *ObjAddr = (uint8 *)ScriptArray->GetData();
				uint8 *ValueAddr = ObjAddr + Index * ElementSize;

				FCDynamicProperty * ElementProperty = GetDynamicPropertyByUEProperty(Inner);

				ElementProperty->m_WriteScriptFunc(L, ElementProperty, ValueAddr, NULL, ObjRef);
                return 1;
			}
		}
	}
    lua_pushnil(L);
    return 1;
}

int FCTArrayWrap::GetAt_wrap(lua_State* L)
{
	return FCTArrayWrap_GetAt_Wrap(L);
}

// SetAt(self, Index, value)
void FCTArrayWrap_SetAt_warp(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
            int Index = lua_tointeger(L, 2) - 1; // lua 从 1开始

			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			int32 Num = ScriptArray->Num();
			if (Index >= 0 && Index < Num)
			{
				FProperty *Inner = ArrayProperty->Inner;
				int ElementSize = Inner->GetSize();
				uint8 *ObjAddr = (uint8 *)ScriptArray->GetData();
				uint8 *ValueAddr = ObjAddr + Index * ElementSize;
				Inner->InitializeValue(ValueAddr);

				FCDynamicProperty* ElementProperty = GetDynamicPropertyByUEProperty(Inner);

				ElementProperty->m_ReadScriptFunc(L, 3, ElementProperty, ValueAddr, NULL, ObjRef);
			}
		}
	}
}

int FCTArrayWrap::SetAt_wrap(lua_State* L)
{
	FCTArrayWrap_SetAt_warp(L);
	return 0;
}

// value = array[key]
int FCTArrayWrap::GetIndex_wrap(lua_State* L)
{
	return FCTArrayWrap_GetAt_Wrap(L);
}

// array[key] = value
int FCTArrayWrap::SetIndex_wrap(lua_State* L)
{
	FCTArrayWrap_SetAt_warp(L);
	return 0;
}

int FCTArrayWrap::Swap_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			int A = lua_tointeger(L, 2) - 1; // lua 从 1开始
			int B = lua_tointeger(L, 3) - 1; // lua 从 1开始

			FScriptArray* ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
			FProperty* Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();
			int Num = ScriptArray->Num();
			if (A >= 0 && B >= 0 && A < Num && B < Num)
			{
				ScriptArray->SwapMemory(A, B, ElementSize);
			}
		}
	}
	return 0;
}

int  TArrayWrap_Add(lua_State* L, FCObjRef* ObjRef, int Idx)
{
	int Index = INDEX_NONE;
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FScriptArray* ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
			FProperty* Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();
			Index = ScriptArray->Add(1, ElementSize);
			uint8* ObjAddr = (uint8*)ScriptArray->GetData();
			uint8* ValueAddr = ObjAddr + Index * ElementSize;
			Inner->InitializeValue(ValueAddr);

			FCDynamicProperty* ElementProperty = GetDynamicPropertyByUEProperty(Inner);

			ElementProperty->m_ReadScriptFunc(L, Idx, ElementProperty, ValueAddr, NULL, ObjRef);
		}
	}
	return Index;
}

int TArrayWrap_Find(lua_State* L, FCObjRef* ObjRef, int Idx)
{
	int Index = INDEX_NONE;
	TArrayCacheValue Value;
	if(Value.ReadValue(L, ObjRef, Idx))
	{
		FProperty* Inner = Value.Inner;
		int ElementSize = Inner->GetSize();
		uint8* ObjAddr = (uint8*)Value.ScriptArray->GetData();
		int Numb = Value.ScriptArray->Num();
		for (int32 i = 0; i < Numb; ++i)
		{
			uint8* ValueAddr = ObjAddr + Index * ElementSize;
			if (Inner->Identical(Value.ElementCache, ValueAddr))
			{
				Index = i;
				break;
			}
		}
	}
	return Index;
}

int FCTArrayWrap::Add_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	int Index = TArrayWrap_Add(L, ObjRef, 2);
	lua_pushinteger(L, Index);
	return 1;
}

int FCTArrayWrap::AddUnique_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	int Index = TArrayWrap_Find(L, ObjRef, 2);
	if (Index == INDEX_NONE)
	{
		Index = TArrayWrap_Add(L, ObjRef, 2);
	}
	lua_pushnumber(L, Index);
	return 1;
}

int FCTArrayWrap::Contains_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	int Index = TArrayWrap_Find(L, ObjRef, 2);
	lua_pushboolean(L, Index != INDEX_NONE);
	return 1;
}

int FCTArrayWrap::Find_wrap(lua_State* L)
{
	// int Find(value)
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	int Index = TArrayWrap_Find(L, ObjRef, 2);
	lua_pushnumber(L, Index);
	return 1;
}

int FCTArrayWrap::Insert_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			int32 Index = lua_tointeger(L, 3) - 1; // lua 从1开始

			FScriptArray* ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
			FProperty* Inner = ArrayProperty->Inner;
			int Numb = ScriptArray->Num();
			int ElementSize = Inner->GetSize();

			if (Index >= 0 && Index < Numb)
			{
				ScriptArray->Insert(Index, 1, ElementSize);
				uint8* ObjAddr = (uint8*)ScriptArray->GetData();
				uint8* ValueAddr = ObjAddr + Index * ElementSize;
				Inner->InitializeValue(ValueAddr);
				FCDynamicProperty* ElementProperty = GetDynamicPropertyByUEProperty(Inner);
				ElementProperty->m_ReadScriptFunc(L, 2, ElementProperty, ValueAddr, NULL, ObjRef);
			}
		}
	}
	return 0;
}

void TArrayWrap_Remove(FCObjRef* ObjRef, int Index)
{
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FScriptArray* ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
			FProperty* Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();
			int Numb = ScriptArray->Num();
			if (Index >= 0 && Index < Numb)
			{
				uint8* ObjAddr = (uint8*)ScriptArray->GetData();
				uint8* ValueAddr = ObjAddr + Index * ElementSize;
				Inner->DestroyValue(ValueAddr);
				ScriptArray->Remove(Index, 1, ElementSize);
			}
		}
	}
}

int FCTArrayWrap::RemoveItem_wrap(lua_State* L)
{
	// RemoveItem(value)
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);

	int RemoveCount = 0;
	TArrayCacheValue Value;
	if(Value.ReadValue(L, ObjRef, 2))
	{
		FScriptArray* ScriptArray = Value.ScriptArray;
		FProperty* Inner = Value.Inner;
		int ElementSize = Inner->GetSize();
		uint8* ObjAddr = (uint8*)Value.ScriptArray->GetData();
		int Numb = Value.ScriptArray->Num();
		for (int32 Index = Numb - 1; Index >= 0; --Index)
		{
			uint8* ValueAddr = ObjAddr + Index * ElementSize;
			if (Inner->Identical(Value.ElementCache, ValueAddr))
			{
				Inner->DestroyValue(ValueAddr);
				ScriptArray->Remove(Index, 1, ElementSize);

				++RemoveCount;
			}
		}
	}
	lua_pushinteger(L, RemoveCount);
	return 1;
}

int FCTArrayWrap::Remove_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			int Index = lua_tointeger(L, 2);

			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			FProperty *Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();
			int Numb = ScriptArray->Num();
			if(Index >= 0 && Index < Numb)
			{
				uint8 *ObjAddr = (uint8 *)ScriptArray->GetData();
				uint8 *ValueAddr = ObjAddr + Index * ElementSize;
				Inner->DestroyValue(ValueAddr);
				ScriptArray->Remove(Index, 1, ElementSize);
			}
		}
	}
	return 0;
}
int FCTArrayWrap::Clear_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			FProperty *Inner = ArrayProperty->Inner;
			TArray_Clear(ScriptArray, Inner);
		}
	}
	return 0;
}

int FCTArrayWrap::Reserve_wrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	int32 Size = lua_tointeger(L, 2);
	if (ObjRef && ObjRef->DynamicProperty && Size > 0)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FScriptArray* ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty* ArrayProperty = (FArrayProperty*)ObjRef->DynamicProperty->Property;
			FProperty* Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();
			ScriptArray->Empty(Size, ElementSize);
		}
	}
	return 0;
}

int FCTArrayWrap::ToList_wrap(lua_State* L)
{
	// 将TArray转换成脚本内置的List<T>
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			FProperty *Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();
			int Numb = ScriptArray->Num();

			FCDynamicProperty* ElementProperty = GetDynamicPropertyByUEProperty(Inner);

            lua_createtable(L, 0, 0);
            int nTableIdx = lua_gettop(L);

			uint8 *ObjAddr = (uint8 *)ScriptArray->GetData();
			uint8 *ValueAddr = ObjAddr;
			for(int Index = 0; Index<Numb; ++Index)
			{
				ValueAddr = ObjAddr + Index * ElementSize;
				ElementProperty->m_WriteScriptFunc(L, ElementProperty, ValueAddr, NULL, NULL);
                lua_rawseti(L, nTableIdx, Index + 1);
			}
            return 1;
		}
	}
    lua_pushnil(L);
	return 1;
}

struct TArraySetListCallbackInfo
{
    FCObjRef* ObjRef;
    FScriptArray* ScriptArray;
    FCDynamicProperty* ElementProperty;
    uint8* ObjAddr;
    int ElementSize;
};

void  TArraySetList_Callback(lua_State* L, int Index, void* UserData)
{
	TArraySetListCallbackInfo*Info = (TArraySetListCallbackInfo*)UserData;
    uint8 *ValueAddr = Info->ObjAddr + Index * Info->ElementSize;
    Info->ElementProperty->m_ReadScriptFunc(L, -2, Info->ElementProperty, ValueAddr, NULL, Info->ObjRef);
}

int FCTArrayWrap::SetList_wrap(lua_State* L)
{
	// 将脚本内置的List<T> 设置到TArray
    int TopIdx = lua_gettop(L);
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->DynamicProperty)
	{
		if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Array)
		{
            int32 Type = lua_rawget(L, 2);
            if (Type != LUA_TTABLE)
            {
                return 0;
            }
            int nTableIdx = 2;
            // 如果不在栈顶上，就压入此表的一份引用到栈顶

			FScriptArray *ScriptArray = (FScriptArray*)ObjRef->GetThisAddr();
			FArrayProperty  *ArrayProperty = (FArrayProperty *)ObjRef->DynamicProperty->Property;
			FProperty *Inner = ArrayProperty->Inner;
			int ElementSize = Inner->GetSize();

			// 需要的话，这里可以优化一下
			FCDynamicProperty* ElementProperty = GetDynamicPropertyByUEProperty(Inner);

			int ScriptListNumb = LoopTable(L, nTableIdx, nullptr, nullptr);
			FCTArrayWrap_SetNumb(ScriptArray, Inner, ScriptListNumb);

            uint8* ObjAddr = (uint8*)ScriptArray->GetData();
			TArraySetListCallbackInfo  Info = { ObjRef,  ScriptArray , ElementProperty , ObjAddr , ElementSize };
            LoopTable(L, nTableIdx, TArraySetList_Callback, &Info);
		}
	}
	return 0;
}