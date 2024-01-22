#include "FCPlaneWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCVectorBaseWrap.h"

static FCExportedClass  GFPlaneWrapClass("FPlane");

void FCPlaneWrap::Register(lua_State* L)
{
    GFPlaneWrapClass.SetCustomRegister();
    luaL_requiref(L, "FPlane", LibOpen_wrap, 1);
    luaL_requiref(L, "Plane", LibOpen_wrap, 1);
}

int FCPlaneWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        { "Copy", Vector_CopyWrap<FPlane> },
        { "CopyFrom", Vector_CopyFromWrap<FPlane> },
        { "Set", Set_wrap },

        { "Add", Vector_AddWrap<FPlane> },
        { "Sub", Vector_SubWrap<FPlane> },
        { "Mul", Vector_MulWrap<FPlane> },
        { "Div", Vector_DivFloatWrap<FPlane> },

        { "Normalize", Vector_NormalizeWrap<FPlane> },
        { "PlaneDot", PlaneDot_wrap },
        { "GetOrigin", GetOrigin_wrap },
        { "GetNormal", GetNormal_wrap },
        { "IntersectLine", IntersectLine_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "X", Vector_GetX<FPlane>, Vector_SetX<FPlane> },
        { "Y", Vector_GetY<FPlane>, Vector_SetY<FPlane> },
        { "Z", Vector_GetZ<FPlane>, Vector_SetZ<FPlane> },
        { "W", Vector_GetW<FPlane>, Vector_SetW<FPlane> },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__add", Vector_Double_Add_Wrap<FPlane> },
        { "__sub", Vector_Double_Sub_Wrap<FPlane> },
        { "__mul", Vector_Double_Mul_Wrap<FPlane> },
        { "__div", Vector_Double_Div_float_Wrap<FPlane> },
        { "__tostring", tostring_wrap },
        { "__gc", FCExportedClass::obj_Delete },
        { "__eq", FCExportedClass::obj_equal },
        { "__index", obj_Index },
        { "__newindex", obj_NewIndex },
        { "__call", obj_New },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GFPlaneWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCPlaneWrap::Set_wrap(lua_State* L)
{
    int NumParams = lua_gettop(L);
    FPlane* A = (FPlane*)VectorBase_GetAddr(L, 1, "FPlane");
    if (A)
    {
        if (NumParams > 1)
            A->X = lua_tonumber(L, 2);
        if (NumParams > 2)
            A->Y = lua_tonumber(L, 3);
        if (NumParams > 3)
            A->Z = lua_tonumber(L, 4);
        if (NumParams > 4)
            A->W = lua_tonumber(L, 5);
    }
    return 0;
}

int FCPlaneWrap::PlaneDot_wrap(lua_State* L)
{
    FPlane* A = (FPlane*)VectorBase_GetAddr(L, 1, "FPlane");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector");
    float  fDist = 0;
    if(A && B)
    {
        fDist = A->PlaneDot(*B);
    }
    FCScript::SetArgValue(L, fDist);
    return 1;
}

int FCPlaneWrap::GetOrigin_wrap(lua_State* L)
{
    FPlane* A = (FPlane*)VectorBase_GetAddr(L, 1, "FPlane");
    FVector Result;
    if(A)
    {
        Result = A->GetOrigin();
    }
    FCScript::SetArgValue(L, Result);
    return 1;
}

int FCPlaneWrap::GetNormal_wrap(lua_State* L)
{
    FPlane* A = (FPlane*)VectorBase_GetAddr(L, 1, "FPlane");
    FVector Result;
    if (A)
    {
        Result = A->GetNormal();
    }
    FCScript::SetArgValue(L, Result);
    return 1;
}

int FCPlaneWrap::IntersectLine_wrap(lua_State* L)
{
    // FVector3 FPlane::IntersectLine(const FVector3 *pBegin, const FVector3 *pRayDir)
    FPlane* A = (FPlane*)VectorBase_GetAddr(L, 1, "FPlane");
    FVector* B = (FVector*)VectorBase_GetAddr(L, 2, "FVector"); // StartPos
    FVector* C = (FVector*)VectorBase_GetAddr(L, 3, "FVector"); // RayDir
    FVector Result;
    if (A && B && C)
    {
        FVector Normal = A->GetNormal();
        float t2 = Normal.Dot(*C);
        if (t2 == 0)
            return false;
        float t = -(Normal.Dot(*B) + A->W) / t2;
        Result = *B + (*C * t);
    }
    FCScript::SetArgValue(L, Result);
    return 1;
}

int FCPlaneWrap::tostring_wrap(lua_State* L)
{
    FPlane* A = (FPlane*)VectorBase_GetAddr(L, 1, "FPlane");
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
int FCPlaneWrap::obj_New(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("FPlane");
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FPlane* V = (FPlane*)FCGetObj::GetIns()->GetPropertyAddr(ObjID);
    if(ParamCount >= 5)
    {
        V->X = lua_tonumber(L, 2);
        V->Y = lua_tonumber(L, 3);
        V->Z = lua_tonumber(L, 4);
        V->W = lua_tonumber(L, 5);
    }
    else if(ParamCount == 4)
    {
        // FPlane(v1, v2, v3)
        FVector* A = (FPlane*)VectorBase_GetAddr(L, 2, "FVector");
        FVector* B = (FPlane*)VectorBase_GetAddr(L, 3, "FVector");
        FVector* C = (FPlane*)VectorBase_GetAddr(L, 4, "FVector");
        if(A && B && C)
        {
            *V = FPlane(*A, *B, *C);
        }
    }
    else if(ParamCount == 3)
    {
        // FPlane(pt, Normal)
        // FPlane(Vector, W)
        FVector* A = (FPlane*)VectorBase_GetAddr(L, 2, "FVector");
        FVector* B = (FPlane*)VectorBase_GetAddr(L, 3, "FVector");
        if(A && B)
        {
            *V = FPlane(*A, *B);
        }
        else if(A)
        {
            float W = lua_tonumber(L, 3);
            *V = FPlane(*A, W);
        }
    }
    else if(ParamCount == 2)
    {
        // FPlane(FVector4)
        FVector4* A = (FVector4*)VectorBase_GetAddr(L, 2, "FVector4");
        if(A)
        {
            *V = FPlane(*A);
        }
    }
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int FCPlaneWrap::obj_Index(lua_State* L)
{
    return GFPlaneWrapClass.DoLibIndex(L);
}
int FCPlaneWrap::obj_NewIndex(lua_State* L)
{
    return GFPlaneWrapClass.DoLibNewIndex(L);
}
