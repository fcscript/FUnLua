#pragma once
#include "FCBrigeHelper.h"

struct OperatorAdd {};
struct OperatorSub {};
struct OperatorMul {};
struct OperatorDiv {};

int VectorBase_SetWrap(lua_State* L, int MemberCount);
void* VectorBase_GetAddr(lua_State* L, int Idx, const char *ClassName = nullptr);

template<class _Ty>
int Vector_SetWrap(lua_State* L)
{
	return VectorBase_SetWrap(L, sizeof(_Ty) / sizeof(float));
}

template<class _Ty>
int Vector_NormalizeWrap(lua_State* L)
{
	FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
	if (ObjRef && ObjRef->IsValid())
	{
		_Ty* V = (_Ty*)ObjRef->GetPropertyAddr();
		V->Normalize();
	}
	return 0;
}

template<class _Ty>
int Vector_GetNormalizedWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty V;
	if (A)
	{
		V = A->GetNormalized();
	}
	FCScript::SetArgValue(L, V);
	return 1;
}

template<class _Ty>
int Vector_IsNormalizedWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	lua_pushboolean(L, A ? A->IsNormalized() : false);
	return 1;
}

template <class _Ty>
int Vector_AddWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A += *B;
	}
	return 0;
}

template <class _Ty>
int Vector_SubWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A -= *B;
	}
	return 0;
}

template <class _Ty>
int Vector_MulWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A *= *B;
	}
	return 0;
}

template <class _Ty>
int Vector_MulFloatWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	float B = lua_tonumber(L, 2);
	if (A) // 必要的话，这里做一个类型检查
	{
		*A *= B;
	}
	return 0;
}

template <class _Ty>
int Vector_DivWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A /= *B;
	}
	return 0;
}

template <class _Ty>
int Vector_DivFloatWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	float B = lua_tonumber(L, 2);
	if (A) // 必要的话，这里做一个类型检查
	{
		*A /= B;
	}
	return 0;
}


template <class _Ty>
int Vector_DotWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	lua_pushnumber(L, A && B ? (*A | *B) : 0);
	return 1;
}


template <class _Ty>
int Vector_AnyCrossWrap(lua_State* L, const _Ty&Type)
{
	const char* ClassName = FCScript::ExtractTypeName(Type);
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);

	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	_Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	if (A && B)
	{
		*V = (*A) ^ (*B);
	}
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

template <class _Ty>
int Vector_CrossWrap(lua_State* L)
{
	// V = Cross(A, B)
	return Vector_AnyCrossWrap(L, _Ty());
}


template <class _Ty>
int Vector_SizeWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	lua_pushnumber(L, A ? A->Size() : 0);
	return 1;
}

template <class _Ty>
int Vector_Size2DWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	lua_pushnumber(L, A ? A->Size2D() : 0);
	return 1;
}

template <class _Ty>
float Vector_SizeSquared(_Ty* V)
{
	return V->SizeSquared();
}
//template <>
//float Vector_SizeSquared(FIntVector* V)
//{
//	return V->X * V->X + V->Y * V->Y + V->Z * V->Z;
//}

template <class _Ty>
int Vector_SizeSquaredWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	lua_pushnumber(L, A ? Vector_SizeSquared(A) : 0);
	return 1;
}

template <class _Ty>
int Vector_SizeSquared2DWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	lua_pushnumber(L, A ? A->SizeSquared2D() : 0);
	return 1;
}

template <class _Ty>
int Vector_DistWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	lua_pushnumber(L, A && B ? _Ty::Distance(*A, *B) : 0);
	return 1;
}

template <class _Ty>
int Vector_Dist2DWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	lua_pushnumber(L, A && B ? _Ty::Dist2D(*A, *B) : 0);
	return 1;
}

template <class _Ty>
int Vector_DistSquaredWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	lua_pushnumber(L, A && B ? _Ty::DistSquared(*A, *B) : 0);
	return 1;
}

template <class _Ty>
int Vector_DistSquared2DWrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2);
	lua_pushnumber(L, A && B ? _Ty::DistSquared2D(*A, *B) : 0);
	return 1;
}

#define VECTOR_MEMBER_OFFSET(t,b) (((t*)0)->b)

template <class _Ty, const int MemberIndex>
int Vector_GetIntValue(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	lua_pushinteger(L, A ? A[MemberIndex] : 0);
	return 1;
}

template <class _Ty, int MemberIndex>
int Vector_GetFloat(lua_State* L)
{
	float* A = (float*)VectorBase_GetAddr(L, 1);
	lua_pushnumber(L, A ? A[MemberIndex] : 0);
	return 1;
}

template <class _Ty, int MemberIndex>
int Vector_SetFloat(lua_State* L)
{
	// (table, key, value)
	float* A = (float*)VectorBase_GetAddr(L, 1);
	float Value = lua_tonumber(L, 3);
	if (A)
	{
		A[MemberIndex] = Value;
	}
	return 0;
}

template <class _Ty, const int MemberIndex>
int Vector_SetIntValue(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	_Ty Value = (_Ty)lua_tointeger(L, 3);
	if (A)
	{
		A[MemberIndex] = Value;
	}
	return 0;
}

template <class _Ty>
void Vector_DoubleOperator(_Ty& V, _Ty& A, _Ty& B, const OperatorAdd &)
{
	V = A + B;
}
template <class _Ty>
void Vector_DoubleOperator(_Ty& V, _Ty& A, _Ty& B, const OperatorSub &)
{
	V = A - B;
}
template <class _Ty>
void Vector_DoubleOperator(_Ty& V, _Ty& A, _Ty& B, const OperatorMul &)
{
	V = A * B;
}
template <class _Ty>
void Vector_DoubleOperator(_Ty& V, _Ty& A, _Ty& B, const OperatorDiv &)
{
	V = A / B;
}

template <class _Ty, class _TyOperatroType>
int Vector_Double_Operator_Wrap(lua_State* L, const char* ClassName, const _TyOperatroType &opType)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);

	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	_Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	Vector_DoubleOperator(*V, *A, *B, opType);
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

template <class _Ty>
int Vector_Double_Add_Wrap(lua_State* L)
{
	// a = b + c
	const char* ClassName = FCScript::ExtractTypeName(_Ty());
	return Vector_Double_Operator_Wrap<_Ty>(L, ClassName, OperatorAdd());
}

template <class _Ty>
int Vector_Double_Sub_Wrap(lua_State* L)
{
	// a = b - c
	const char* ClassName = FCScript::ExtractTypeName(_Ty());
	return Vector_Double_Operator_Wrap<_Ty>(L, ClassName, OperatorSub());
}

template <class _Ty>
int Vector_Double_Mul_Wrap(lua_State* L)
{
	// a = b * c
	const char* ClassName = FCScript::ExtractTypeName(_Ty());
	return Vector_Double_Operator_Wrap<_Ty>(L, ClassName, OperatorMul());
}
template <class _Ty>
int Vector_Double_Mul_float_Wrap(lua_State* L)
{
	// a = b * c
	const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	float B = lua_tonumber(L, 2);

	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	_Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	if (A)
		*V = *A * B;
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

template <class _Ty>
int Vector_Double_Div_Wrap(lua_State* L)
{
	// a = b / c
	const char* ClassName = FCScript::ExtractTypeName(_Ty());
	return Vector_Double_Operator_Wrap<_Ty>(L, ClassName, OperatorDiv());
}
template <class _Ty>
int Vector_Double_Div_float_Wrap(lua_State* L)
{
	const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	float B = lua_tonumber(L, 2);

	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	_Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	if (A)
		*V = *A / B;
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

template <class _Ty>
int Vector_unm_Wrap(lua_State* L, const char *ClassName)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	_Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	*V = -(*A);
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

template <class _Ty>
int Vector_ToOrientationRotator_Wrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FRotator");
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	FRotator* V = (FRotator*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	*V = A->ToOrientationRotator();
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

template <class _Ty>
int Vector_ToOrientationQuat_Wrap(lua_State* L)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1);
	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FQuat");
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	FQuat* V = (FQuat *)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	*V = A->ToOrientationQuat();
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}
