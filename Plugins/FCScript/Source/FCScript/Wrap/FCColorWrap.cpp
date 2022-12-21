#include "FCColorWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFColorWrapClass("FColor");

void FCColorWrap::Register(lua_State* L)
{
    GFColorWrapClass.SetCustomRegister();
    luaL_requiref(L, "FColor", LibOpen_wrap, 1);
    luaL_requiref(L, "Color", LibOpen_wrap, 1);
}

int FCColorWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Set", Set_wrap },
        { "Add", Add_wrap },
        { "ToLinearColor", ToLinearColor_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "R", GetR_wrap, SetR_wrap },
        { "G", GetG_wrap, SetG_wrap },
        { "B", GetB_wrap, SetB_wrap },
        { "A", GetA_wrap, SetA_wrap },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", double_add_wrap },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFColorWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCColorWrap::GetR_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    lua_pushinteger(L, A ? A->R : 0);
    return 1;
}
int FCColorWrap::GetG_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    lua_pushinteger(L, A ? A->G : 0);
    return 1;
}
int FCColorWrap::GetB_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    lua_pushinteger(L, A ? A->B : 0);
    return 1;
}
int FCColorWrap::GetA_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    lua_pushinteger(L, A ? A->A : 0);
    return 1;
}

int FCColorWrap::SetR_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    if (A)
        A->R = (uint8)lua_tointeger(L, 3);
    return 0;
}
int FCColorWrap::SetG_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    if (A)
        A->G = (uint8)lua_tointeger(L, 3);
    return 0;
}
int FCColorWrap::SetB_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    if (A)
        A->B = (uint8)lua_tointeger(L, 3);
    return 0;
}
int FCColorWrap::SetA_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    if (A)
        A->A = (uint8)lua_tointeger(L, 3);
    return 0;
}

#define FCOLOR_SAFE_SET(NumParams, V, idx)  if(NumParams >= idx){ V = (uint8)lua_tonumber(L,idx); }

int FCColorWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    FCOLOR_SAFE_SET(NumParams, A->R, 2);
    FCOLOR_SAFE_SET(NumParams, A->G, 3);
    FCOLOR_SAFE_SET(NumParams, A->B, 4);
    FCOLOR_SAFE_SET(NumParams, A->A, 5);
    return 0;
}

int FCColorWrap::Add_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    FColor* B = (FColor*)VectorBase_GetAddr(L, 2);
    if (A && B)
    {
        *A += *B;
    }
    return 0;
}

int FCColorWrap::ToLinearColor_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);

    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FLinearColor");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FLinearColor* V = (FLinearColor*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    if(A)
        *V = A->ReinterpretAsLinear();

    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

int FCColorWrap::double_add_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1);
    FColor* B = (FColor*)VectorBase_GetAddr(L, 2);

    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FColor");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FColor* V = (FColor*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    // R, G, B, A
    if (A && B)
    {
        *V = *A;
        *V += *B;
    }
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}

int FCColorWrap::tostring_wrap(lua_State* L)
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
int FCColorWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FColor");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FColor* V = (FColor*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    // R, G, B, A
    V->R = (uint8)lua_tonumber(L, 2);
    V->G = (uint8)lua_tonumber(L, 3);
    V->B = (uint8)lua_tonumber(L, 4);
    V->A = (uint8)lua_tonumber(L, 5);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCColorWrap::obj_Index(lua_State* L)
{
    return GFColorWrapClass.DoLibIndex(L);
}
int FCColorWrap::obj_NewIndex(lua_State* L)
{
    return GFColorWrapClass.DoLibNewIndex(L);
}
