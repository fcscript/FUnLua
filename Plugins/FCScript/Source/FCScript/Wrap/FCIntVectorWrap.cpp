#include "FCIntVectorWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFIntVectorWrapClass("FIntVector");

void FCIntVectorWrap::Register(lua_State* L)
{
    GFIntVectorWrapClass.SetCustomRegister();
    luaL_requiref(L, "FIntVector", LibOpen_wrap, 1);
    luaL_requiref(L, "IntVector", LibOpen_wrap, 1);
}

int FCIntVectorWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Set", Set_wrap },
        { "SizeSquared", Vector_SizeSquaredWrap<FIntVector> },
        { "Add", Vector_AddWrap<FIntVector> },
        { "Sub", Vector_SubWrap<FIntVector> },
        { "Mul", mul_wrap },
        { "Div", div_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetIntValue<int, 0>, Vector_SetIntValue<int, 0> },
        { "Y", Vector_GetIntValue<int, 1>, Vector_SetIntValue<int, 1> },
        { "Z", Vector_GetIntValue<int, 2>, Vector_SetIntValue<int, 2> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", Vector_Double_Add_Wrap<FIntVector> },
        { "__sub", Vector_Double_Sub_Wrap<FIntVector> },
        { "__mul", double_mul_wrap },
        { "__div", double_div_wrap },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFIntVectorWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCIntVectorWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FIntVector* A = (FIntVector*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        A->X = lua_tointeger(L, 2);
        A->Y = lua_tointeger(L, 3);
        A->Z = lua_tointeger(L, 4);
    }
    return 0;
}

int FCIntVectorWrap::mul_wrap(lua_State* L)
{
    FIntVector* A = (FIntVector*)VectorBase_GetAddr(L, 1, "FIntVector");
    int B = lua_tointeger(L, 2);
    if (A)
    {
        *A *= B;
    }
    return 0;
}
int FCIntVectorWrap::div_wrap(lua_State* L)
{
    FIntVector* A = (FIntVector*)VectorBase_GetAddr(L, 1, "FIntVector");
    int B = lua_tointeger(L, 2);
    if (A)
    {
        *A /= B;
    }
    return 0;
}
int FCIntVectorWrap::double_mul_wrap(lua_State* L)
{
    FIntVector* A = (FIntVector*)VectorBase_GetAddr(L, 1, "FIntVector");
    int B = lua_tointeger(L, 2);
    FIntVector V;
    if (A)
    {
        V = *A * B;
    }
    FCScript::SetArgValue(L, V);
    return 1;
}
int FCIntVectorWrap::double_div_wrap(lua_State* L)
{
    FIntVector* A = (FIntVector*)VectorBase_GetAddr(L, 1, "FIntVector");
    int B = lua_tointeger(L, 2);
    FIntVector V;
    if (A)
    {
        V = *A / B;
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCIntVectorWrap::tostring_wrap(lua_State* L)
{
    FIntVector* A = (FIntVector*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(X=%d,Y=%d,Z=%d)"), A, A->X, A->Y, A->Z);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}
int FCIntVectorWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FIntVector");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FIntVector* V = (FIntVector*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    V->X = (uint8)lua_tonumber(L, 2);
    V->Y = (uint8)lua_tonumber(L, 3);
    V->Z = (uint8)lua_tonumber(L, 4);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName.c_str());
    return 1;
}
int FCIntVectorWrap::obj_Index(lua_State* L)
{
    return GFIntVectorWrapClass.DoLibIndex(L);
}
int FCIntVectorWrap::obj_NewIndex(lua_State* L)
{
    return GFIntVectorWrapClass.DoLibNewIndex(L);
}
