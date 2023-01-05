#include "FCVectorBaseWrap.h"

#include "FCObjectManager.h"
#include "FCGetObj.h"

int VectorBase_SetWrap(lua_State* L, int MemberCount, const char* ClassName)
{
    float *v = (float*)VectorBase_GetAddr(L, 1, ClassName);
    if (v)
    {
        int ParamCount = lua_gettop(L);
        if (ParamCount > MemberCount)
            ParamCount = MemberCount;
        for (int i = 0; i < ParamCount; ++i)
        {
            v[i] = lua_tonumber(L, i + 2);
        }
    }
    return 0;
}

void* VectorBase_GetAddr(lua_State* L, int Idx, const char* ClassName)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, Idx);
    if (ClassName)
    {
        FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
        if (ObjRef && ObjRef->IsValid() && ObjRef->ClassDesc == ClassDesc)
            return ObjRef->GetPropertyAddr();
        else
            return nullptr;
    }
    else
    {
        return ObjRef && ObjRef->IsValid() ? ObjRef->GetPropertyAddr() : nullptr;
    }
}
