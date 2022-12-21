#include "FCVector4Wrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFVector4WrapClass("FVector4");

void FCVector4Wrap::Register(lua_State* L)
{
    GFVector4WrapClass.SetCustomRegister();
    luaL_requiref(L, "FVector4", LibOpen_wrap, 1);
    luaL_requiref(L, "Vector4", LibOpen_wrap, 1);
}

int FCVector4Wrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Set", Vector_SetWrap<FVector4> },
        { "Add", Vector_AddWrap<FVector4> },
        { "Sub", Vector_SubWrap<FVector4> },
        { "Mul", Vector_MulWrap<FVector4> },
        { "Div", Vector_DivWrap<FVector4> },
        { "Dot", Dot4_Wrap },
        { "Cross", Vector_CrossWrap<FVector4> },
        { "Size", Vector_SizeWrap<FVector4> },
        { "SizeSquared", Vector_SizeSquaredWrap<FVector4> },

        { "Size3", Size3_Wrap },
        { "SizeSquared3", SizeSquared3_Wrap },
        { "Dot3", Dot3_Wrap },
        { "Dot4", Dot4_Wrap },

        { "ToRotator", Vector_ToOrientationRotator_Wrap<FVector4> },
        { "ToQuat", Vector_ToOrientationQuat_Wrap<FVector4> },

        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetFloat<FVector4, 0>, Vector_SetFloat<FVector4, 0> },
        { "Y", Vector_GetFloat<FVector4, 1>, Vector_SetFloat<FVector4, 1> },
        { "Z", Vector_GetFloat<FVector4, 2>, Vector_SetFloat<FVector4, 2> },
        { "W", Vector_GetFloat<FVector4, 3>, Vector_SetFloat<FVector4, 3> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", double_add_wrap },
        { "__sub", double_sub_wrap },
        { "__mul", double_mul_wrap },
        { "__div", double_div_wrap },
        { "__unm", num_wrap }, // -FVector4
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFVector4WrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCVector4Wrap::double_add_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector4>(L, "FVector4", OperatorAdd());
}

int FCVector4Wrap::double_sub_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector4>(L, "FVector4", OperatorSub());
}

int FCVector4Wrap::double_mul_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector4>(L, "FVector4", OperatorMul());
}

int FCVector4Wrap::double_div_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector4>(L, "FVector4", OperatorDiv());
}

int FCVector4Wrap::num_wrap(lua_State* L)
{
    return Vector_unm_Wrap<FVector4>(L, "FVector4");
}

int FCVector4Wrap::Size3_Wrap(lua_State* L)
{
    FVector4* A = (FVector4*)VectorBase_GetAddr(L, 1);
    lua_pushnumber(L, A ? A->Size3() : 0);
    return 1;
}

int FCVector4Wrap::SizeSquared3_Wrap(lua_State* L)
{
    FVector4* A = (FVector4*)VectorBase_GetAddr(L, 1);
    lua_pushnumber(L, A ? A->SizeSquared3() : 0);
    return 1;
}

int FCVector4Wrap::Dot3_Wrap(lua_State* L)
{
    FVector4* A = (FVector4*)VectorBase_GetAddr(L, 1);
    FVector4* B = (FVector4*)VectorBase_GetAddr(L, 2);
    if(A && B)
        lua_pushnumber(L, A->X * B->X + A->Y * B->Y + A->Z * B->Z);
    else
        lua_pushnumber(L, 0);
    return 1;
}

int FCVector4Wrap::Dot4_Wrap(lua_State* L)
{
    FVector4* A = (FVector4*)VectorBase_GetAddr(L, 1);
    FVector4* B = (FVector4*)VectorBase_GetAddr(L, 2);
    if (A && B)
        lua_pushnumber(L, A->X * B->X + A->Y * B->Y + A->Z * B->Z + A->W * B->W);
    else
        lua_pushnumber(L, 0);
    return 1;
}

int FCVector4Wrap::tostring_wrap(lua_State* L)
{
    FVector4* A = (FVector4*)VectorBase_GetAddr(L, 1);
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

int FCVector4Wrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FVector4");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FVector4* V = (FVector4*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    const char* ClassName = lua_tostring(L, 1);
    V->X = lua_tonumber(L, 2);
    V->Y = lua_tonumber(L, 3);
    V->Z = lua_tonumber(L, 4);
    V->W = lua_tonumber(L, 5);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

int FCVector4Wrap::obj_Index(lua_State* L)
{
    return GFVector4WrapClass.DoLibIndex(L);
}

int FCVector4Wrap::obj_NewIndex(lua_State* L)
{
    return GFVector4WrapClass.DoLibNewIndex(L);
}
