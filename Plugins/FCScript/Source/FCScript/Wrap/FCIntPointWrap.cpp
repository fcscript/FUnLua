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
        { "Set", Set_wrap },
        { "Add", Vector_AddWrap<FIntPoint> },
        { "Sub", Vector_SubWrap<FIntPoint> },
        { "Mul", Vector_MulWrap<FIntPoint> },
        { "Div", Vector_DivWrap<FIntPoint> },
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
        { "__add", Vector_Double_Add_Wrap<FIntPoint> },
        { "__sub", Vector_Double_Sub_Wrap<FIntPoint> },
        { "__mul", Vector_Double_Mul_Wrap<FIntPoint> },
        { "__div", Vector_Double_Div_Wrap<FIntPoint> },
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
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName.c_str());
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
