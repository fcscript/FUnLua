#include "FCIntPointWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFIntPointWrapClass("FIntPoint");

void FCIntPointWrap::Register(lua_State* L)
{
    GFIntPointWrapClass.SetCustomRegister();
    luaL_requiref(L, "FIntPoint", LibOpen_wrap, 1);
    luaL_requiref(L, "IntPoint", LibOpen_wrap, 1);
}

int FCIntPointWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Copy", Vector_CopyWrap<FIntPoint> },
        { "CopyFrom", Vector_CopyFromWrap<FIntPoint> },
        { "Set", Set_wrap },
        { "Add", Add_wrap },
        { "Sub", Sub_wrap },
        { "Mul", Mul_wrap },
        { "Div", Div_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetIntValue<int, 0>, Vector_SetIntValue<int, 0> },
        { "Y", Vector_GetIntValue<int, 1>, Vector_SetIntValue<int, 1> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", double_add_wrap },
        { "__sub", double_sub_wrap },
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
    GFIntPointWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCIntPointWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        A->X = lua_tointeger(L, 2);
        A->Y = lua_tointeger(L, 3);
    }
    return 0;
}

int FCIntPointWrap::Add_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    FIntPoint* B = (FIntPoint*)VectorBase_GetAddr(L, 2, "FIntPoint");
    if(A && B)
    {
        A->X += B->X;
        A->Y += B->Y;
    }
    return 0;
}

int FCIntPointWrap::Sub_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    FIntPoint* B = (FIntPoint*)VectorBase_GetAddr(L, 2, "FIntPoint");
    if (A && B)
    {
        A->X -= B->X;
        A->Y -= B->Y;
    }
    return 0;
}

int FCIntPointWrap::Mul_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    int B = lua_tointeger(L, 2);
    if (A)
    {
        A->X *= B;
        A->Y *= B;
    }
    return 0;
}

int FCIntPointWrap::Div_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    int B = lua_tointeger(L, 2);
    if (A && B != 0)
    {
        A->X /= B;
        A->Y /= B;
    }
    return 0;
}

int FCIntPointWrap::double_add_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    FIntPoint* B = (FIntPoint*)VectorBase_GetAddr(L, 2, "FIntPoint");
    FIntPoint  V;
    if(A && B)
    {
        V.X = A->X + B->X;
        V.Y = A->X + B->Y;
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCIntPointWrap::double_sub_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    FIntPoint* B = (FIntPoint*)VectorBase_GetAddr(L, 2, "FIntPoint");
    FIntPoint  V;
    if (A && B)
    {
        V.X = A->X - B->X;
        V.Y = A->X - B->Y;
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCIntPointWrap::double_mul_wrap(lua_State* L)
{
    // Dot ???
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    FIntPoint* B = (FIntPoint*)VectorBase_GetAddr(L, 2, "FIntPoint");
    lua_pushinteger(L, A && B ? (A->X * B->X + A->Y * B->Y): 0);
    return 1;
}

int FCIntPointWrap::double_div_wrap(lua_State* L)
{
    FIntPoint* A = (FIntPoint*)VectorBase_GetAddr(L, 1, "FIntPoint");
    int B = lua_tointeger(L, 2);
    FIntPoint  V;
    if(A && B != 0)
    {
        V.X = A->X / B;
        V.Y = A->Y / B;
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCIntPointWrap::tostring_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(R=%d,G=%d,B=%d,A=%d)"), A, A->R, A->G, A->B, A->A);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}
int FCIntPointWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FIntPoint");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FIntPoint* V = (FIntPoint*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    V->X = (uint8)lua_tonumber(L, 2);
    V->Y = (uint8)lua_tonumber(L, 3);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCIntPointWrap::obj_Index(lua_State* L)
{
    return GFIntPointWrapClass.DoLibIndex(L);
}
int FCIntPointWrap::obj_NewIndex(lua_State* L)
{
    return GFIntPointWrapClass.DoLibNewIndex(L);
}
