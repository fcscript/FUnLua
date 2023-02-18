#include "FCLinearColorWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFLinearColorWrapClass("FLinearColor");

void FCLinearColorWrap::Register(lua_State* L)
{
    GFLinearColorWrapClass.SetCustomRegister();
    luaL_requiref(L, "FLinearColor", LibOpen_wrap, 1);
    luaL_requiref(L, "LinearColor", LibOpen_wrap, 1);
}

int FCLinearColorWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Copy", Vector_CopyWrap<FLinearColor> },
        { "CopyFrom", Vector_CopyFromWrap<FLinearColor> },
        { "Set", Set_wrap },
        {" FromSRGBColor", FromSRGBColor_wrap },
        {" ToFColor", ToFColor_wrap },
        {" Clamp", Clamp_wrap },

        { "Add", Vector_AddWrap<FLinearColor> },
        { "Sub", Vector_SubWrap<FLinearColor> },
        { "Mul", Vector_MulWrap<FLinearColor> },
        { "Div", Vector_DivWrap<FLinearColor> },
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
        { "__add", Vector_Double_Add_Wrap<FLinearColor> },
        { "__sub", Vector_Double_Sub_Wrap<FLinearColor> },
        { "__mul", Vector_Double_Mul_Wrap<FLinearColor> },
        { "__div", Vector_Double_Div_Wrap<FLinearColor> },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFLinearColorWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCLinearColorWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        if(NumParams > 1)
            A->R = lua_tonumber(L, 2);
        if(NumParams > 2)
            A->G = lua_tonumber(L, 3);
        if(NumParams > 3)
            A->B = lua_tonumber(L, 4);
        if(NumParams > 4)
            A->A = lua_tonumber(L, 5);
    }
    return 0;
}

int FCLinearColorWrap::FromSRGBColor_wrap(lua_State* L)
{
    FColor* A = (FColor*)VectorBase_GetAddr(L, 1, "FColor");
    FLinearColor V;
    if (A)
    {
        V = FLinearColor::FromSRGBColor(*A);
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCLinearColorWrap::ToFColor_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    bool bSRGB = lua_toboolean(L, 2) != 0;
    FColor V;    
    if (A)
    {
        V = A->ToFColor(bSRGB);
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCLinearColorWrap::Clamp_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    FLinearColor V;
    if (A)
    {
        V = A->GetClamped();
    }
    FCScript::SetArgValue(L, V);
    return 1;
}

int FCLinearColorWrap::GetR_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    lua_pushnumber(L, A ? A->R : 0);
    return 1;
}
int FCLinearColorWrap::GetG_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    lua_pushnumber(L, A ? A->G : 0);
    return 1;
}
int FCLinearColorWrap::GetB_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    lua_pushnumber(L, A ? A->B : 0);
    return 1;
}
int FCLinearColorWrap::GetA_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    lua_pushnumber(L, A ? A->A : 0);
    return 1;
}
int FCLinearColorWrap::SetR_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    if(A)
    {
        A->R = lua_tonumber(L, 3);
    }
    return 0;
}
int FCLinearColorWrap::SetG_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    if (A)
    {
        A->G = lua_tonumber(L, 3);
    }
    return 0;
}
int FCLinearColorWrap::SetB_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    if (A)
    {
        A->B = lua_tonumber(L, 3);
    }
    return 0;
}
int FCLinearColorWrap::SetA_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1, "FLinearColor");
    if (A)
    {
        A->A = lua_tonumber(L, 3);
    }
    return 0;
}

int FCLinearColorWrap::tostring_wrap(lua_State* L)
{
    FLinearColor* A = (FLinearColor*)VectorBase_GetAddr(L, 1);
    if (A)
    {
        FString  Str = FString::Printf(TEXT("%p(R=%f,G=%f,B=%f, A=%f)"), A, A->R, A->G, A->B, A->A);
        lua_pushstring(L, TCHAR_TO_UTF8(*Str));
    }
    else
    {
        lua_pushfstring(L, "nil object");
    }
    return 1;
}
int FCLinearColorWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FLinearColor");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FLinearColor* V = (FLinearColor*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    V->R = lua_tonumber(L, 2);
    V->G = lua_tonumber(L, 3);
    V->B = lua_tonumber(L, 4);
    V->A = lua_tonumber(L, 5);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCLinearColorWrap::obj_Index(lua_State* L)
{
    return GFLinearColorWrapClass.DoLibIndex(L);
}
int FCLinearColorWrap::obj_NewIndex(lua_State* L)
{
    return GFLinearColorWrapClass.DoLibNewIndex(L);
}
