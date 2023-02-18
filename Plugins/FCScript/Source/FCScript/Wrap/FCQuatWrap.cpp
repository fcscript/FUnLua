#include "FCQuatWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFQuatWrapClass("FQuat");

void FCQuatWrap::Register(lua_State* L)
{
    GFQuatWrapClass.SetCustomRegister();
    luaL_requiref(L, "FQuat", LibOpen_wrap, 1);
    luaL_requiref(L, "Quat", LibOpen_wrap, 1);
}

int FCQuatWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Copy", Vector_CopyWrap<FQuat> },
        { "CopyFrom", Vector_CopyFromWrap<FQuat> },
        { "Set", Set_wrap },

        { "Add", Vector_AddWrap<FQuat> },
        { "Sub", Vector_SubWrap<FQuat> },
        { "Mul", Vector_MulWrap<FQuat> },
        { "Div", Vector_DivFloatWrap<FQuat> },

        { "Normalize", Vector_NormalizeWrap<FQuat> },
        { "IsNormalized", Vector_IsNormalizedWrap<FQuat> },
        { "GetNormalized", GetNormalized_wrap },
        { "Size", Vector_SizeWrap<FQuat> },
        { "SizeSquared", Vector_SizeSquaredWrap<FQuat> },
        { "ToAxisAndAngle", ToAxisAndAngle_wrap },
        { "Inverse", Inverse_wrap },
        { "RotateVector", RotateVector_wrap },
        { "UnrotateVector", UnrotateVector_wrap },

        { "GetAxisX", GetAxisX_wrap },
        { "GetAxisY", GetAxisY_wrap },
        { "GetAxisZ", GetAxisZ_wrap },

        { "GetForwardVector", GetForwardVector_wrap },
        { "GetRightVector", GetRightVector_wrap },
        { "GetUpVector", GetUpVector_wrap },

        { "Slerp", Slerp_wrap },
        { "ToEuler", ToEuler_wrap },
        { "ToRotator", ToRotator_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetX<FQuat>, Vector_SetX<FQuat> },
        { "Y", Vector_GetY<FQuat>, Vector_SetY<FQuat> },
        { "Z", Vector_GetZ<FQuat>, Vector_SetZ<FQuat> },
        { "W", Vector_GetW<FQuat>, Vector_SetW<FQuat> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", Vector_Double_Add_Wrap<FQuat> },
        { "__sub", Vector_Double_Sub_Wrap<FQuat> },
        { "__mul", Vector_Double_Mul_Wrap<FQuat> },
        { "__div", Vector_Double_Div_float_Wrap<FQuat> },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFQuatWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCQuatWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    if (A)
    {
        if (NumParams > 1)
            A->X = lua_tonumber(L, 2);
        if (NumParams > 2)
            A->Y = lua_tonumber(L, 3);
        if (NumParams > 3)
            A->Z = lua_tonumber(L, 4);
        if (NumParams > 4)
            A->W = lua_tonumber(L, 5);
    }
    return 0;
}

int FCQuatWrap::GetNormalized_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FQuat  V;
    if (A)
    {
        V = A->GetNormalized();
    }
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::ToAxisAndAngle_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    if (A)
    {
        FVector Axis; 
        float Angle;
        A->ToAxisAndAngle(Axis, Angle);
        FCScript::SetArgValue(L, Axis);
        FCScript::SetArgValue(L, Angle);
        return 2;
    }
    return 0;
}
int FCQuatWrap::Inverse_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FQuat V;
    if (A)
    {
        V = A->Inverse();
    }
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::RotateVector_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    FVector V;
    if (A && B)
    {
        V = A->RotateVector(*B);
    }
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::UnrotateVector_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    FVector V;
    if (A)
    {
        V = A->UnrotateVector(*B);
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCQuatWrap::GetAxisX_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FVector V;
    if (A)
        V = A->GetAxisX();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::GetAxisY_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FVector V;
    if (A)
        V = A->GetAxisY();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::GetAxisZ_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FVector V;
    if (A)
        V = A->GetAxisZ();
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCQuatWrap::GetForwardVector_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    if (A)
    {
        FCScript::SetArgValue(L, A->GetForwardVector());
        return 1;
    }
    return 0;
}
int FCQuatWrap::GetRightVector_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    if (A)
    {
        FCScript::SetArgValue(L, A->GetRightVector());
        return 1;
    }
    return 0;
}
int FCQuatWrap::GetUpVector_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    if (A)
    {
        FCScript::SetArgValue(L, A->GetUpVector());
        return 1;
    }
    return 0;
}

int FCQuatWrap::Slerp_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FQuat* B = (FQuat*)VectorBase_GetAddr(L, 2, "FQuat");
    float Slerp = lua_tonumber(L, 3);
    // static FORCEINLINE FQuat Slerp(const FQuat &Quat1, const FQuat &Quat2, float Slerp)
    FQuat V;
    if (A && B)
        V = FQuat::Slerp(*A, *B, Slerp);
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::ToEuler_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FVector V;
    if(A)
        V = A->Euler();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCQuatWrap::ToRotator_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1, "FQuat");
    FRotator V;
    if (A)
    {
        V = A->Rotator();
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCQuatWrap::tostring_wrap(lua_State* L)
{
    FQuat* A = (FQuat*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(X=%f,Y=%f,Z=%f,W=%f)"), A, A->X, A->Y, A->Z, A->W);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}
int FCQuatWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FQuat");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FQuat* V = (FQuat*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    V->X = lua_tonumber(L, 2);
    V->Y = lua_tonumber(L, 3);
    V->Z = lua_tonumber(L, 4);
    V->W = lua_tonumber(L, 5);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCQuatWrap::obj_Index(lua_State* L)
{
    return GFQuatWrapClass.DoLibIndex(L);
}
int FCQuatWrap::obj_NewIndex(lua_State* L)
{
    return GFQuatWrapClass.DoLibNewIndex(L);
}
