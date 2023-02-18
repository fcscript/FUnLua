#include "FCVector3Wrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFVector3WrapClass("FVector");

void FCVector3Wrap::Register(lua_State* L)
{
    GFVector3WrapClass.SetCustomRegister();
    luaL_requiref(L, "FVector", LibOpen_wrap, 1);
    luaL_requiref(L, "Vector", LibOpen_wrap, 1);
}

int FCVector3Wrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Copy", Vector_CopyWrap<FVector> },
        { "CopyFrom", Vector_CopyFromWrap<FVector> },
        { "Set", Vector_SetWrap<FVector> },
        { "Normalize", Vector_NormalizeWrap<FVector> },
        { "IsNormalized", Vector_IsNormalizedWrap<FVector> },
        { "Add", Vector_AddWrap<FVector> },
        { "Sub", Vector_SubWrap<FVector> },
        { "Mul", Vector_MulWrap<FVector> },
        { "Div", Vector_DivWrap<FVector> },
        { "Dot", Vector_DotWrap<FVector> },
        { "Cross", Vector_CrossWrap<FVector> },
        { "Size", Vector_SizeWrap<FVector> },
        { "SizeSquared", Vector_SizeSquaredWrap<FVector> },
        { "Dist", Vector_DistWrap<FVector> },
        { "DistSquared", Vector_DistSquaredWrap<FVector> },

        { "Size2D", Vector_Size2DWrap<FVector> },
        { "SizeSquared2D", Vector_SizeSquared2DWrap<FVector> },
        { "Dist2D", Vector_Dist2DWrap<FVector> },
        { "DistSquared2D", Vector_DistSquared2DWrap<FVector> },

        { "ToRotator", Vector_ToOrientationRotator_Wrap<FVector> },
        { "ToQuat", Vector_ToOrientationQuat_Wrap<FVector> },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetX<FVector>, Vector_SetX<FVector> },
        { "Y", Vector_GetY<FVector>, Vector_SetY<FVector> },
        { "Z", Vector_GetZ<FVector>, Vector_SetZ<FVector> },
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
    GFVector3WrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCVector3Wrap::double_add_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector", OperatorAdd());
}

int FCVector3Wrap::double_sub_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector", OperatorSub());
}

int FCVector3Wrap::double_mul_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector", OperatorMul());
}

int FCVector3Wrap::double_div_wrap(lua_State* L)
{
    return Vector_Double_Operator_Wrap<FVector2D>(L, "FVector", OperatorDiv());
}

int FCVector3Wrap::num_wrap(lua_State* L)
{
    return Vector_unm_Wrap<FVector2D>(L, "FVector");
}

int FCVector3Wrap::tostring_wrap(lua_State* L)
{
    FVector* A = (FVector*)VectorBase_GetAddr(L, 1, "FVector");
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(X=%f,Y=%f,Z=%f)"), A, A->X, A->Y, A->Z);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}

int FCVector3Wrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FVector");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FVector* V = (FVector*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    const char* ClassName = lua_tostring(L, 1);
    V->X = lua_tonumber(L, 2);
    V->Y = lua_tonumber(L, 3);
    V->Z = lua_tonumber(L, 4);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

int FCVector3Wrap::obj_Index(lua_State* L)
{
    return GFVector3WrapClass.DoLibIndex(L);
}

int FCVector3Wrap::obj_NewIndex(lua_State* L)
{
    return GFVector3WrapClass.DoLibNewIndex(L);
}
