#pragma once
#include "FCBrigeHelper.h"

struct OperatorAdd {};
struct OperatorSub {};
struct OperatorMul {};
struct OperatorDiv {};

int VectorBase_SetWrap(lua_State* L, int MemberCount, const char * ClassName);
void* VectorBase_GetAddr(lua_State* L, int Idx, const char *ClassName = nullptr);

template<class _Ty>
int Vector_SetWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	return VectorBase_SetWrap(L, sizeof(_Ty) / sizeof(float), ClassName);
}

template<class _Ty>
int Vector_CopyWrap(lua_State* L)
{
    // A:Copy(B) 有参数，就修改B, 并返回B
    // A:Copy() 没有参数，就返回一个新的
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    _Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
    if(A)
    {
        if(B)
        {
            *B = *A;
            lua_pushvalue(L, 2);
        }
        else
        {
            _Ty V(*A);
            FCScript::SetArgValue(L, V);
        }
        return 1;
    }
    return 0;
}

template<class _Ty>
int Vector_CopyFromWrap(lua_State* L)
{
    // Struct.CopyFrom(A, B) 有参数，就修改A, 并返回A
    // Struct.CopyFrom(A)    没有参数，就相当于重置A
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    _Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
    if (A)
    {
        if (B)
        {
            *A = *B;
        }
        else
        {
            *A = _Ty();
        }
        lua_pushvalue(L, 1);
        return 1;
    }
    return 0;
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	lua_pushboolean(L, A ? A->IsNormalized() : false);
	return 1;
}

template <class _Ty>
int Vector_AddWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A += *B;
	}
	return 0;
}

template <class _Ty>
int Vector_SubWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A -= *B;
	}
	return 0;
}

template <class _Ty>
int Vector_MulWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A *= *B;
	}
	return 0;
}

template <class _Ty>
int Vector_MulFloatWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	if (A && B) // 必要的话，这里做一个类型检查
	{
		*A /= *B;
	}
	return 0;
}

template <class _Ty>
int Vector_DivFloatWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	lua_pushnumber(L, A ? A->Size() : 0);
	return 1;
}

template <class _Ty>
int Vector_Size2DWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	lua_pushnumber(L, A ? Vector_SizeSquared(A) : 0);
	return 1;
}

template <class _Ty>
int Vector_SizeSquared2DWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	lua_pushnumber(L, A ? A->SizeSquared2D() : 0);
	return 1;
}

template <class _Ty>
int Vector_DistWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	lua_pushnumber(L, A && B ? _Ty::Distance(*A, *B) : 0);
	return 1;
}

template <class _Ty>
int Vector_Dist2DWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	lua_pushnumber(L, A && B ? _Ty::Dist2D(*A, *B) : 0);
	return 1;
}

template <class _Ty>
int Vector_DistSquaredWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	lua_pushnumber(L, A && B ? _Ty::DistSquared(*A, *B) : 0);
	return 1;
}

template <class _Ty>
int Vector_DistSquared2DWrap(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	lua_pushnumber(L, A && B ? _Ty::DistSquared2D(*A, *B) : 0);
	return 1;
}

#define VECTOR_MEMBER_OFFSET(t,b) (((t*)0)->b)

template <class _Ty, const int MemberIndex>
int Vector_GetIntValue(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	lua_pushinteger(L, A ? A[MemberIndex] : 0);
	return 1;
}

template <class _Ty>
int Vector_GetX(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    lua_pushnumber(L, A ? A->X : 0);
    return 1;
}

template <class _Ty>
int Vector_GetY(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    lua_pushnumber(L, A ? A->Y : 0);
    return 1;
}

template <class _Ty>
int Vector_GetZ(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    lua_pushnumber(L, A ? A->Z : 0);
    return 1;
}

template <class _Ty>
int Vector_GetW(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    lua_pushnumber(L, A ? A->W : 0);
    return 1;
}

template <class _Ty>
int Vector_SetX(lua_State* L)
{
    // A->X = value
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    if(A)
    {
        A->X = lua_tonumber(L, 3);
    }
    return 0;
}

template <class _Ty>
int Vector_SetY(lua_State* L)
{
    // A->X = value
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    if (A)
    {
        A->Y = lua_tonumber(L, 3);
    }
    return 0;
}

template <class _Ty>
int Vector_SetZ(lua_State* L)
{
    // A->X = value
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    if (A)
    {
        A->Z = lua_tonumber(L, 3);
    }
    return 0;
}

template <class _Ty>
int Vector_SetW(lua_State* L)
{
    // A->X = value
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    if (A)
    {
        A->W = lua_tonumber(L, 3);
    }
    return 0;
}

template <class _Ty, class _TyMember, int MemberOffset>
int Vector_GetMember(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    unsigned char* A = (unsigned char*)VectorBase_GetAddr(L, 1, ClassName);
    _TyMember  V;
    if(A)
    {
        V = *((_TyMember *)(A + MemberOffset));
    }
    FCScript::SetArgValue(L, V);
    return 1;
}
template <class _Ty, class _TyMember, int MemberOffset>
int Vector_SetMember(lua_State* L)
{
    // Class.Member = value
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    unsigned char* A = (unsigned char*)VectorBase_GetAddr(L, 1, ClassName);    
    if(A)
    {
        _TyMember *V = (_TyMember *)(A + MemberOffset);
        FCScript::GetArgValue(L, 2, *V);
    }
    return 0;
}

template <class _Ty, int MemberIndex>
int Vector_GetFloat(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    float* A = (float*)VectorBase_GetAddr(L, 1, ClassName);
	lua_pushnumber(L, A ? A[MemberIndex] : 0);
	return 1;
}

template <class _Ty, int MemberIndex>
int Vector_SetFloat(lua_State* L)
{
	// (table, key, value)
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
    FLargeWorldCoordinatesReal* A = (FLargeWorldCoordinatesReal*)VectorBase_GetAddr(L, 1, ClassName);
	if (A)
	{
		A[MemberIndex] = lua_tonumber(L, 3);
	}
	return 0;
}

template <class _Ty, const int MemberIndex>
int Vector_SetIntValue(lua_State* L)
{
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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

template <class _Ty>
void Vector_DoubleFloat(_Ty& V, _Ty& A, float B, const OperatorAdd&)
{
    //V = A + B;
}
template <class _Ty>
void Vector_DoubleFloat(_Ty& V, _Ty& A, float B, const OperatorSub&)
{
    //V = A - B;
}
template <class _Ty>
void Vector_DoubleFloat(_Ty& V, _Ty& A, float B, const OperatorMul&)
{
    V = A * B;
}
template <class _Ty>
void Vector_DoubleFloat(_Ty& V, _Ty& A, float B, const OperatorDiv&)
{
    V = A / B;
}

template <class _Ty, class _TyOperatroType>
int Vector_Double_Operator_Base_Wrap(lua_State* L, const char* ClassName, const _TyOperatroType& opType)
{
    _Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
    _Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    _Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    if (A)
    {
        if (B)
        {
            Vector_DoubleOperator(*V, *A, *B, opType);
        }
        else
        {
            float Arg2 = lua_tonumber(L, 2);
            Vector_DoubleFloat(*V, *A, Arg2, opType);
        }
    }
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

template <class _Ty, class _TyOperatroType>
int Vector_Double_Operator_Wrap(lua_State* L, const char* ClassName, const _TyOperatroType &opType)
{
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	_Ty* B = (_Ty*)VectorBase_GetAddr(L, 2, ClassName);
	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	_Ty* V = (_Ty*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    if(A && B)
    {
        Vector_DoubleOperator(*V, *A, *B, opType);
    }
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
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
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
    const char* ClassName = FCScript::ExtractTypeName(_Ty());
	_Ty* A = (_Ty*)VectorBase_GetAddr(L, 1, ClassName);
	FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FQuat");
	int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
	FQuat* V = (FQuat *)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
	*V = A->ToOrientationQuat();
	FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
	return 1;
}

#define DEFIND_CLASS_UOBJECT_VALUE(ClassName, Property)  \
int GetWrap_##ClassName##Property(lua_State* L) \
{ \
    ClassName* A = (ClassName*)VectorBase_GetAddr(L, 1, #ClassName);\
    FCScript::SetArgValue(L, (UObject*)A->Property);\
    return 1; \
}\
int SetWrap_##ClassName##Property(lua_State* L) \
{ \
    ClassName* A = (ClassName*)VectorBase_GetAddr(L, 1, #ClassName);\
    A->Property = FCScript::CastArgValue_UObject(L, 3, A->Property); \
    return 0; \
}

#define DEFINED_CLASS_VALUE(ClassName, Property)  \
int GetWrap_##ClassName##Property(lua_State* L) \
{ \
    ClassName* A = (ClassName*)VectorBase_GetAddr(L, 1, #ClassName);\
    FCScript::SetArgValue(L, A->Property);\
    return 1; \
}\
int SetWrap_##ClassName##Property(lua_State* L) \
{ \
    ClassName* A = (ClassName*)VectorBase_GetAddr(L, 1, #ClassName);\
    FCScript::GetArgValue(L, 3, A->Property); \
    return 0; \
}

#define DEFINED_CLASS_BIT_VALUE(ClassName, Property)  \
int GetBitWrap_##ClassName##Property(lua_State* L) \
{ \
    ClassName* A = (ClassName*)VectorBase_GetAddr(L, 1, #ClassName);\
    bool V = A->Property;\
    FCScript::SetArgValue(L, V);\
    return 1; \
}\
int SetBitWrap_##ClassName##Property(lua_State* L) \
{ \
    ClassName* A = (ClassName*)VectorBase_GetAddr(L, 1, #ClassName);\
    bool V = false; \
    FCScript::GetArgValue(L, 3, V); \
    A->Property = V; \
    return 0; \
}

#define  CLASS_VALUE_FUNC(ClassName, Property) { #Property, GetWrap_##ClassName##Property, SetWrap_##ClassName##Property }
#define  CLASS_BIT_VALUE_FUNC(ClassName, Property) { #Property, GetBitWrap_##ClassName##Property, SetBitWrap_##ClassName##Property }
