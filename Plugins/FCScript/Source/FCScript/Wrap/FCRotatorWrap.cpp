#include "FCRotatorWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFRotatorWrapClass("FRotator");

void FCRotatorWrap::Register(lua_State* L)
{
    GFRotatorWrapClass.SetCustomRegister();
    luaL_requiref(L, "FRotator", LibOpen_wrap, 1);
    luaL_requiref(L, "Rotator", LibOpen_wrap, 1);
}

int FCRotatorWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Set", Set_wrap },

        { "Add", Vector_AddWrap<FRotator> },
        { "Sub", Vector_SubWrap<FRotator> },
        { "Mul", Vector_MulFloatWrap<FRotator> },

        { "Normalize", Vector_NormalizeWrap<FRotator> },
        { "GetNormalized", Vector_GetNormalizedWrap<FRotator> },

        { "GetRightVector", GetRightVector_wrap },
        { "GetUpVector", GetUpVector_wrap },
        { "GetUnitAxis", GetUnitAxis_wrap },
        { "RotateVector", RotateVector_wrap },
        { "UnrotateVector", UnrotateVector_wrap },
        { "Clamp", Clamp_wrap },

        { "GetForwardVector", GetForwardVector_wrap },
        { "ToVector", ToVector_wrap },
        { "ToEuler", ToEuler_wrap },
        { "ToQuat", ToQuat_wrap },
        { "Inverse", Inverse_wrap },

        { "MakeFromEuler", MakeFromEuler_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "Pitch", Vector_GetFloat<FRotator, 0>, Vector_SetFloat<FRotator, 0> },
        { "Yaw",   Vector_GetFloat<FRotator, 1>, Vector_SetFloat<FRotator, 1> },
        { "Roll",  Vector_GetFloat<FRotator, 2>, Vector_SetFloat<FRotator, 2> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", Vector_Double_Add_Wrap<FRotator> },
        { "__sub", Vector_Double_Sub_Wrap<FRotator> },
        { "__mul", Vector_Double_Mul_float_Wrap<FRotator> },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFRotatorWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCRotatorWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    if (A)
    {
        if (NumParams > 1)
            A->Pitch = lua_tonumber(L, 2);
        if (NumParams > 2)
            A->Yaw = lua_tonumber(L, 3);
        if (NumParams > 3)
            A->Roll = lua_tonumber(L, 4);
    }
    return 0;
}

int FCRotatorWrap::GetRightVector_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector V;
    if(A)
        V = FRotationMatrix(*A).GetScaledAxis(EAxis::Y);
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::GetUpVector_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector V;
    if (A)
        V = FRotationMatrix(*A).GetScaledAxis(EAxis::Z);
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::GetUnitAxis_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    EAxis::Type  InType = (EAxis::Type)lua_tointeger(L, 2);
    FVector V;
    if (A)
        V = FRotationMatrix(*A).GetScaledAxis(InType);
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::RotateVector_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    FVector V;
    if(A && B)
        V = A->RotateVector(*B);
    FCScript::SetArgValue(L, V);
    return 1;

}
int FCRotatorWrap::UnrotateVector_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    FVector V;
    if (A && B)
        V = A->UnrotateVector(*B);
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::Clamp_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FRotator V;
    if (A)
        V = A->Clamp();
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCRotatorWrap::GetForwardVector_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector V;
    if (A)
        V = A->Vector();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::ToVector_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector V;
    if (A)
        V = A->Vector();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::ToEuler_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FVector V;
    if (A)
        V = A->Euler();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::ToQuat_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FQuat V;
    if (A)
        V = A->Quaternion();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::Inverse_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1, "FRotator");
    FRotator V;
    if (A)
        V = A->GetInverse();
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCRotatorWrap::MakeFromEuler_wrap(lua_State* L)
{
    FVector* A = (FVector*)VectorBase_GetAddr(L, 1, "FRotator");
    FRotator V;
    if (A)
        V = FRotator::MakeFromEuler(*A);
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCRotatorWrap::tostring_wrap(lua_State* L)
{
    FRotator* A = (FRotator*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(Pitch=%f,Yaw=%f,Z=%f,Roll=%f)"), A, A->Pitch, A->Yaw, A->Roll);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}
int FCRotatorWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FRotator");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FRotator* V = (FRotator*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    V->Pitch = lua_tonumber(L, 2);
    V->Yaw = lua_tonumber(L, 3);
    V->Roll = lua_tonumber(L, 4);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCRotatorWrap::obj_Index(lua_State* L)
{
    return GFRotatorWrapClass.DoLibIndex(L);
}
int FCRotatorWrap::obj_NewIndex(lua_State* L)
{
    return GFRotatorWrapClass.DoLibNewIndex(L);
}
