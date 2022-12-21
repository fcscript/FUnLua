#include "FCTransformWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFTransformWrapClass("FTransform");

void FCTransformWrap::Register(lua_State* L)
{
    GFTransformWrapClass.SetCustomRegister();
    luaL_requiref(L, "FTransform", LibOpen_wrap, 1);
    luaL_requiref(L, "Transform", LibOpen_wrap, 1);
}

int FCTransformWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Add", Vector_AddWrap<FTransform> },
        { "Mul", Vector_MulWrap<FTransform> },

        { "Blend", Blend_wrap },
        { "Inverse", Inverse_wrap },
        { "TransformPosition", TransformPosition_wrap },
        { "TransformPositionNoScale", TransformPositionNoScale_wrap },
        { "InverseTransformPosition", InverseTransformPosition_wrap },
        { "InverseTransformPositionNoScale", InverseTransformPositionNoScale_wrap },
        { "TransformVector", TransformVector_wrap },
        { "TransformVectorNoScale", TransformVectorNoScale_wrap },
        { "InverseTransformVector", InverseTransformVector_wrap },
        { "InverseTransformVectorNoScale", InverseTransformVectorNoScale_wrap },
        { "TransformRotation", TransformRotation_wrap },
        { "InverseTransformRotation", InverseTransformRotation_wrap },

        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", Vector_Double_Add_Wrap<FTransform> },
        { "__mul", Vector_Double_Mul_Wrap<FTransform> },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFTransformWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCTransformWrap::Blend_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    if (NumParams > 3)
    {
        // A = Blend(B, C, Alpha);
        FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
        FTransform* B = (FTransform*)VectorBase_GetAddr(L, 2, "FTransform");
        FTransform* C = (FTransform*)VectorBase_GetAddr(L, 3, "FTransform");
        float Alpha = lua_tonumber(L, 4);
        if (A && B && C)
        {
            A->Blend(*B, *C, Alpha);
        }
    }
    else
    {
        // A->BlendWith(*B, Alpha);
        FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
        FTransform* B = (FTransform*)VectorBase_GetAddr(L, 2, "FTransform");
        float Alpha = lua_tonumber(L, 3);
        if (A && B)
        {
            A->BlendWith(*B, Alpha);
        }
    }
    return 0;
}
int FCTransformWrap::Inverse_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    if (A)
    {
        FCScript::SetArgValue(L, A->Inverse());
        return 1;
    }
    return 0;
}
int FCTransformWrap::TransformPosition_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->TransformPosition(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::TransformPositionNoScale_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->InverseTransformPositionNoScale(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::InverseTransformPosition_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->InverseTransformPosition(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::InverseTransformPositionNoScale_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->InverseTransformPositionNoScale(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::TransformVector_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->TransformVector(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::TransformVectorNoScale_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->TransformVectorNoScale(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::InverseTransformVector_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->InverseTransformVector(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::InverseTransformVectorNoScale_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->InverseTransformVectorNoScale(*B));
        return 1;
    }
    return 0;

}
int FCTransformWrap::TransformRotation_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FQuat* B = (FQuat*)VectorBase_GetAddr(L, 2, "FQuat");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->TransformRotation(*B));
        return 1;
    }
    return 0;
}
int FCTransformWrap::InverseTransformRotation_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1, "FTransform");
    FQuat* B = (FQuat*)VectorBase_GetAddr(L, 2, "FQuat");
    if (A && B)
    {
        FCScript::SetArgValue(L, A->InverseTransformRotation(*B));
        return 1;
    }
    return 0;
}

int FCTransformWrap::tostring_wrap(lua_State* L)
{
    FTransform* A = (FTransform*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(FTransform)"), A);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}
int FCTransformWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FTransform");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FTransform* V = (FTransform*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);

    FQuat* Quat = (FQuat*)VectorBase_GetAddr(L, 2, "FQuat");
    if (Quat)
        V->SetRotation(*Quat);
    FVector* Pos = (FVector*)VectorBase_GetAddr(L, 3, "FVector");
    if (Pos)
        V->SetTranslation(*Pos);
    FVector* Scale = (FVector*)VectorBase_GetAddr(L, 4, "FVector");
    if (Scale)
        V->SetScale3D(*Scale);

    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCTransformWrap::obj_Index(lua_State* L)
{
    return GFTransformWrapClass.DoLibIndex(L);
}
int FCTransformWrap::obj_NewIndex(lua_State* L)
{
    return GFTransformWrapClass.DoLibNewIndex(L);
}
