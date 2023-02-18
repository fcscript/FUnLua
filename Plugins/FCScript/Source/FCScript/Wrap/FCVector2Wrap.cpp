#include "FCVector2Wrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFVector2WrapClass("FVector2D");

void FCVector2Wrap::Register(lua_State* L)
{
    GFVector2WrapClass.SetCustomRegister();
    luaL_requiref(L, "FVector2D", LibOpen_wrap, 1);
    luaL_requiref(L, "Vector2D", LibOpen_wrap, 1);
}

int FCVector2Wrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Copy", Vector_CopyWrap<FVector2D> },
        { "CopyFrom", Vector_CopyFromWrap<FVector2D> },
        { "Set", Vector_SetWrap<FVector2D> },
        { "Normalize", Vector_NormalizeWrap<FVector2D> },
        { "IsNormalized", IsNormalized_wrap },
        { "Add", Vector_AddWrap<FVector2D> },
        { "Sub", Vector_SubWrap<FVector2D> },
        { "Mul", Vector_MulWrap<FVector2D> },
        { "Div", Vector_DivWrap<FVector2D> },
        { "Dot", Vector_DotWrap<FVector2D> },
        { "Cross", Cross_wrap },
        { "Size", Vector_SizeWrap<FVector2D> },
        { "SizeSquared", Vector_SizeSquaredWrap<FVector2D> },
        { "Dist", Vector_DistWrap<FVector2D> },
        { "DistSquared", Vector_DistSquaredWrap<FVector2D> },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetX<FVector2D>, Vector_SetX<FVector2D> },
        { "Y", Vector_GetY<FVector2D>, Vector_SetY<FVector2D> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", double_add_wrap },
        { "__sub", double_sub_wrap },
        { "__mul", double_mul_wrap },
        { "__div", double_div_wrap },
        { "__unm", num_wrap }, // -FVector2D
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFVector2WrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCVector2Wrap::IsNormalized_wrap(lua_State* L)
{
    FVector2D* A = (FVector2D*)VectorBase_GetAddr(L, 1, "FVector2D");
    if(A)
    {
        bool bNormalized = FMath::Abs(1.f - A->SizeSquared()) < THRESH_VECTOR_NORMALIZED;
        lua_pushboolean(L, bNormalized);
    }
    else
    {
        lua_pushboolean(L, false);
    }
    return 1;
}

int FCVector2Wrap::Cross_wrap(lua_State* L)
{
    FVector2D* A = (FVector2D*)VectorBase_GetAddr(L, 1, "FVector2D");
    FVector2D* B = (FVector2D*)VectorBase_GetAddr(L, 2, "FVector2D");
    lua_pushnumber(L, A && B ? (*A ^ *B) : 0);
    return 1;
}

int FCVector2Wrap::double_add_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector2D", OperatorAdd());
}

int FCVector2Wrap::double_sub_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector2D", OperatorSub());
}

int FCVector2Wrap::double_mul_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector2D", OperatorMul());
}

int FCVector2Wrap::double_div_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector2D", OperatorDiv());
}

int FCVector2Wrap::num_wrap(lua_State* L)
{
    return Vector_unm_Wrap<FVector2D>(L, "FVector2D");
}

int FCVector2Wrap::tostring_wrap(lua_State* L)
{
    FVector2D* A = (FVector2D*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(X=%f,Y=%f)"), A, A->X, A->Y);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}

int FCVector2Wrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FVector2D");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FVector2D* V = (FVector2D*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    const char* ClassName = lua_tostring(L, 1);
    V->X = lua_tonumber(L, 2);
    V->Y = lua_tonumber(L, 3);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

int FCVector2Wrap::obj_Index(lua_State* L)
{
    return GFVector2WrapClass.DoLibIndex(L);
}

int FCVector2Wrap::obj_NewIndex(lua_State* L)
{
    return GFVector2WrapClass.DoLibNewIndex(L);
}
