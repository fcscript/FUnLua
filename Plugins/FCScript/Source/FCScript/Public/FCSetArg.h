#pragma once

#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject.h"
#include "FCTypeName.h"

#include "../LuaCore/LuaHeader.h"

FCSCRIPT_API void FC_SetArgValue_CppPtr(lua_State* L, const void* CppPtr);
FCSCRIPT_API void FC_SetArgValue_ByName(lua_State* L, const void* ValueAddr, const char* ClassName);
FCSCRIPT_API void FC_SetArgValue_Object(lua_State* L, const UObject* Object);
FCSCRIPT_API void FC_PushBindLuaValue(lua_State* L, int64 ObjID, const char *ClassName);
FCSCRIPT_API bool GlbRegisterClass(lua_State* L, const char* ClassName);
FCSCRIPT_API void FC_PushArray(lua_State* L, const void *ArrayData, int ArrayLen, const char *InnerType);

namespace FCScript
{
	template<class _Ty>
	FORCEINLINE void SetArgValue(lua_State* L, _Ty)
	{
        lua_pushnil(L);
    }
    template<class _Ty>
    inline void SetArgValue(lua_State* L, _Ty* value)
    {
		FC_SetArgValue_CppPtr(L, value);
    }
    template<class _Ty>
    inline void SetArgValue(lua_State* L, const _Ty* value)
    {
        FC_SetArgValue_CppPtr(L, value);
    }
	FORCEINLINE void SetArgValue(lua_State* L, int8 value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, bool value)
	{
        lua_pushboolean(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, char value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, int16 value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, uint16 value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, int value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, uint32 value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, int64 value)
	{
        lua_pushinteger(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, float value)
	{
        lua_pushnumber(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, double value)
	{
        lua_pushnumber(L, value);
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FString &value)
	{
        lua_pushstring(L, TCHAR_TO_UTF8(*value));
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FName &value)
	{
		FString Name(value.ToString());
        lua_pushstring(L, TCHAR_TO_UTF8(*Name));
	}

	FORCEINLINE void SetArgValue(lua_State* L, const FIntPoint& value)
	{
		FC_SetArgValue_ByName(L, &value, "FIntPoint");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FIntVector& value)
	{
		FC_SetArgValue_ByName(L, &value, "FIntVector");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FRotator& value)
	{
		FC_SetArgValue_ByName(L, &value, "FRotator");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FQuat& value)
    {
        FC_SetArgValue_ByName(L, &value, "FQuat");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FTransform& value)
    {
        FC_SetArgValue_ByName(L, &value, "FTransform");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FLinearColor& value)
    {
        FC_SetArgValue_ByName(L, &value, "FLinearColor");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FColor& value)
    {
        FC_SetArgValue_ByName(L, &value, "FColor");
	}
	FORCEINLINE void SetArgValue(lua_State* L, const FPlane& value)
    {
        FC_SetArgValue_ByName(L, &value, "FPlane");
	}
    FORCEINLINE void SetArgValue(lua_State* L, const FVector& value)
    {
        FC_SetArgValue_ByName(L, &value, "FVector");
    }
    FORCEINLINE void SetArgValue(lua_State* L, const FVector2D& value)
    {
        FC_SetArgValue_ByName(L, &value, "FVector2D");
    }
    FORCEINLINE void SetArgValue(lua_State* L, const FVector4& value)
    {
        FC_SetArgValue_ByName(L, &value, "FVector4");
    }
    FORCEINLINE void SetArgValue(lua_State* L, const FGuid& value)
    {
        FC_SetArgValue_ByName(L, &value, "FGuid");
    }

    template <class _Ty>
    FORCEINLINE void SetArgValue(lua_State* L, const TArray<_Ty> &Array )
    {
        const char *InnerName = FCScript::ExtractTypeName(_Ty());
        FC_PushArray(L, Array.GetData(), Array.Num(), InnerName);
    }

	FORCEINLINE void SetArgValue(lua_State* L, const UObject*& value)
	{
		FC_SetArgValue_Object(L, value);
	}
	FORCEINLINE void PushUObject(lua_State* L, const UObject* value)
	{
		FC_SetArgValue_Object(L, value);
	}
	FORCEINLINE void PushBindObjRef(lua_State* L, int64 ObjID, const char* ClassName)
	{
		FC_PushBindLuaValue(L, ObjID, ClassName);
	}
}