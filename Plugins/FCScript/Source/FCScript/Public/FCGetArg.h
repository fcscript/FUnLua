#pragma once

#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

#include "../LuaCore/LuaHeader.h"

FCSCRIPT_API void* FC_GetArgValue_CppPtr(lua_State* L, int Index);
FCSCRIPT_API void* FC_GetCppObjectPtr(lua_State* L, int Index);
FCSCRIPT_API void  FC_GetArgValue_ByName(lua_State* L, int Index, void* ValueAddr, const char* ClassName);
FCSCRIPT_API UObject* FC_GetArgValue_Object(lua_State* L, int Index);
FCSCRIPT_API UStruct* FC_GetArgValue_UStruct(lua_State* L, int Index);
FCSCRIPT_API UObject* FC_GetArgValue_UStructOrUObject(lua_State* L, int Index);
FCSCRIPT_API void *FC_GetArgRefObjPtr(lua_State* L, int Index);
FCSCRIPT_API int64 FC_GetArgObjID(lua_State* L, int Index);

FORCEINLINE bool FC_Get_IsUObject(const UObject*) { return true; }
FORCEINLINE bool FC_Get_IsUObject(void*) { return false; }

namespace FCScript
{
	template<class _Ty>
    FORCEINLINE void GetArgValue(lua_State* L, int Index, _Ty & Value)
	{
		//Value = _Ty(); // default(_Ty);
        Value = (_Ty)lua_tointeger(L, Index);  // default is enum
    }
    template<class _Ty>
    FORCEINLINE void GetArgValue(lua_State* L, int Index, _Ty*& Value)
    {
        if(FC_Set_IsUObject(Value))
            Value = (_Ty *)FC_GetArgValue_Object(L, Index);
        else
            Value = (_Ty*)FC_GetCppObjectPtr(L, Index);
    }
    template<class _Ty>
    FORCEINLINE void GetArgValue(lua_State* L, int Index, const _Ty*& Value)
    {
        if (FC_Set_IsUObject(Value))
            Value = (_Ty*)FC_GetArgValue_Object(L, Index);
        else
            Value = (_Ty*)FC_GetCppObjectPtr(L, Index);
    }

    template<class _Ty>
    FORCEINLINE _Ty *CastArgValue_UObject(lua_State* L, int Index, _Ty*Value)
    {
        return Cast<_Ty>(FC_GetArgValue_Object(L, Index));
    }

    FORCEINLINE void GetArgValue(lua_State* L, int Index, int8 & Value)
	{
        Value = (int8)lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, uint8& Value)
    {
        Value = (uint8)lua_tointeger(L, Index);
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, bool & Value)
	{
        Value = lua_toboolean(L, Index) != 0;
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, char & Value)
	{
        Value = (char)lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, int16 & Value)
	{
        Value = (int16)lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, uint16 & Value)
	{
        Value = (uint16)lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, int & Value)
	{
        Value = (int)lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, uint32 & Value)
	{
        Value = (uint32)lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, int64 & Value)
	{
        Value = lua_tointeger(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, float & Value)
	{
        Value = (float)lua_tonumber(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, double & Value)
	{
        Value = lua_tonumber(L, Index);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FString & Value)
	{
        const char *str = lua_tostring(L, Index);
        if(!str)
            str = "";
        Value = UTF8_TO_TCHAR(str);
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FName & Value)
	{
        const char* str = lua_tostring(L, Index);
        if (!str)
            str = "";
        Value = str;
	}
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FIntPoint& Value)
    {
        FC_GetArgValue_ByName(L, Index, &Value, "FIntPoint");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FIntVector& Value)
    {
        FC_GetArgValue_ByName(L, Index, &Value, "FIntVector");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FRotator & Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FRotator");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FQuat& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FQuat");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FTransform& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FTransform");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FLinearColor& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FLinearColor");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FColor& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FColor");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FPlane& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FPlane");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FVector& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FVector");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FVector2D& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FVector2D");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FVector4& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FVector4");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, FGuid& Value)
    {
		FC_GetArgValue_ByName(L, Index, &Value, "FGuid");
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, UObject*& Value)
    {
        Value = FC_GetArgValue_Object(L, Index);
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, AActor*& Value)
    {
        Value = Cast<AActor>(FC_GetArgValue_Object(L, Index));
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, APawn*& Value)
    {
        Value = Cast<APawn>(FC_GetArgValue_Object(L, Index));
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, ACharacter*& Value)
    {
        Value = Cast<ACharacter>(FC_GetArgValue_Object(L, Index));
    }
    FORCEINLINE void GetArgValue(lua_State* L, int Index, APlayerController*& Value)
    {
        Value = Cast<APlayerController>(FC_GetArgValue_Object(L, Index));
    }
    FORCEINLINE UObject* GetUObject(lua_State* L, int Index)
    {
        return FC_GetArgValue_Object(L, Index);
    }
    FORCEINLINE UStruct* GetUStruct(lua_State* L, int Index)
    {
        return FC_GetArgValue_UStruct(L, Index);
    }
    FORCEINLINE UObject* GetUStructOrUObject(lua_State* L, int Index)
    {
        return FC_GetArgValue_UStructOrUObject(L, Index);
    }
    FORCEINLINE void* GetObjRefPtr(lua_State* L, int Index)
    {
        return FC_GetArgRefObjPtr(L, Index);
    }
    FORCEINLINE int64 GetObjID(lua_State* L, int Index)
    {
        return FC_GetArgObjID(L, Index);
    }
}