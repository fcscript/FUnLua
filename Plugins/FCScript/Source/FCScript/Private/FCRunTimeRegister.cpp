#include "FCRunTimeRegister.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"
#include "FCDynamicOverrideFunc.h"
#include "FCPropertyType.h"

#include "FCCallUEFunc.h"
#include "FCCore.h"
#include "../LuaCore/LuaContext.h"
#include "../LuaCore/CallLua.h"
#include "FCBrigeBase.h"


//---------------------------------------------------------------------------------

void SetTableForClass(lua_State* L, const char* Name)
{
    lua_setglobal(L, Name);
}

int Enum_Index(lua_State* L)
{
    int32 NumParams = lua_gettop(L);
    lua_pushstring(L, "__name");        // 3
    lua_rawget(L, 1);                   // 3

    const char* pcsEnumName = lua_tostring(L, -1);
    const char* pcsValueName = lua_tostring(L, 2);
    const FDynamicEnum *EnumClass = GetScriptContext()->RegisterEnum(pcsEnumName);
    int Value = 0;
    if (EnumClass)
    {
        Value = EnumClass->FindEnumValue(pcsValueName);
    }
    lua_pop(L, 1);

    // 缓存写入table表，下次就不用再次查询C++接口中了，提升性能
    lua_pushvalue(L, 2);
    lua_pushinteger(L, Value);
    lua_rawset(L, 1);

    // 返回值
    lua_pushinteger(L, Value);

    // 返回值，如果有返回值，就是1？没有就是0?
    return 1;
}

int Enum_Delete(lua_State* L)
{
    lua_pushstring(L, "__name");
    int32 Type = lua_rawget(L, 1);
    if (Type == LUA_TSTRING)
    {
        const char* EnumName = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    return 0;
}

int32 Class_Index(lua_State* L)
{
    return Global_GetUProperty(L);
}

int ScriptStruct_Copy(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));
    UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct; // 确认不会为空
    
    // A:Copy(B) 有参数，就修改B, 并返回B
    // A:Copy() 没有参数，就返回一个新的
    // 必要的话，检查一下来源的类型
    FCObjRef* A = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCObjRef* B = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
    if (A && A->IsValid())
    {
        if (B && B->IsValid())
        {
            if(A->ClassDesc == B->ClassDesc)
            {
                ScriptStruct->CopyScriptStruct(B->GetThisAddr(), A->GetThisAddr());
                lua_pushvalue(L, 2);
                return 1;
            }
            ReportLuaError(L, "unsame class, failed to copy struct");
            return 0;
        }
        else
        {
            // 构建一个默认的，再拷贝, 这个地方不安全，理论上需要释放所有引用的子对象
            int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
            B = FCGetObj::GetIns()->FindValue(ObjID);
            ScriptStruct->CopyScriptStruct(A->GetThisAddr(), B->GetThisAddr());
            FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
            return 1;
        }
    }
    return 0;
}

int ScriptStruct_CopyFrom(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));
    UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct; // 确认不会为空

    // Struct.CopyFrom(A, B) 有参数，就修改A, 并返回A
    // Struct.CopyFrom(A)    没有参数，就相当于重置A
    // 必要的话，检查一下来源的类型
    FCObjRef* A = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCObjRef* B = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
    if (A && A->IsValid())
    {
        if (B && B->IsValid())
        {
            if (A->ClassDesc == B->ClassDesc)
            {
                ScriptStruct->CopyScriptStruct(A->GetThisAddr(), B->GetThisAddr());
                lua_pushvalue(L, 2);
                return 1;
            }
            ReportLuaError(L, "unsame class, failed to copy struct");
            return 0;
        }
        else
        {
            // 构建一个默认的，再拷贝, 这个地方不安全，理论上需要释放所有引用的子对象
            ScriptStruct->DestroyStruct(A->GetPropertyAddr(), 1);
            ScriptStruct->InitializeStruct(A->GetPropertyAddr(), 1);
            lua_pushvalue(L, 1);
            return 1;
        }
    }
    return 0;
}

int ScriptStruct_Copy_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    // A:Copy(B) 有参数，就修改B, 并返回B
    // A:Copy() 没有参数，就返回一个新的
    // 必要的话，检查一下来源的类型
    FCObjRef* A = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCObjRef* B = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
    if (A && A->IsValid())
    {
        FCDynamicClassDesc* ClassDesc = A->ClassDesc;
        UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct; // 确认不会为空
        if (B && B->IsValid())
        {
            if (A->ClassDesc == B->ClassDesc)
            {
                ScriptStruct->CopyScriptStruct(B->GetThisAddr(), A->GetThisAddr());
                lua_pushvalue(L, 2);
                return 1;
            }
            ReportLuaError(L, "unsame class, failed to copy struct");
            return 0;
        }
        else
        {
            // 构建一个默认的，再拷贝, 这个地方不安全，理论上需要释放所有引用的子对象
            int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
            B = FCGetObj::GetIns()->FindValue(ObjID);
            ScriptStruct->CopyScriptStruct(A->GetThisAddr(), B->GetThisAddr());
            FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
            return 1;
        }
    }
    return 0;
}

int ScriptStruct_CopyFrom_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    // Struct.CopyFrom(A, B) 有参数，就修改A, 并返回A
    // Struct.CopyFrom(A)    没有参数，就相当于重置A
    // 必要的话，检查一下来源的类型
    FCObjRef* A = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCObjRef* B = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
    if (A && A->IsValid())
    {
        FCDynamicClassDesc* ClassDesc = A->ClassDesc;
        UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct; // 确认不会为空
        if (B && B->IsValid())
        {
            if (A->ClassDesc == B->ClassDesc)
            {
                ScriptStruct->CopyScriptStruct(A->GetThisAddr(), B->GetThisAddr());
                lua_pushvalue(L, 2);
                return 1;
            }
            ReportLuaError(L, "unsame class, failed to copy struct");
            return 0;
        }
        else
        {
            // 构建一个默认的，再拷贝, 这个地方不安全，理论上需要释放所有引用的子对象
            ScriptStruct->DestroyStruct(A->GetPropertyAddr(), 1);
            ScriptStruct->InitializeStruct(A->GetPropertyAddr(), 1);
            lua_pushvalue(L, 1);
            return 1;
        }
    }
    return 0;
}

int ScriptStruct_Compare(lua_State* L)
{
    // Compare(A, B)
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));
    UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct;

    FCObjRef* A = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCObjRef* B = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
    if (A && B && A->IsValid() && B->IsValid())
    {
        if (A->GetStructProperty() == B->GetStructProperty())
        {
            bool bResult = A && B ? ScriptStruct->CompareScriptStruct(A->GetThisAddr(), B->GetThisAddr(), 0) : false;
            lua_pushboolean(L, bResult);
            return 1;
        }
    }
    lua_pushboolean(L, A == nullptr && B == nullptr);
    return 1;
}
int ScriptStruct_Delete(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));
    UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct;

    int64 ObjID = FCScript::GetObjID(L, 1);
    FCGetObj::GetIns()->DeleteValue(ObjID);

    return 0;
}
const char* GetUETypeNameFromLua(lua_State* L, int Idx)
{
    int32 n = lua_getmetatable(L, Idx);
    if (n == 1)
    {
        lua_pushstring(L, "__name");        // 3
        int ClassType = lua_rawget(L, -2);                 // 3
        const char* UETypeName = lua_tostring(L, -1);
        return UETypeName;
    }
    else
    {
        return nullptr;
    }
}

const char* GetPropertyType(lua_State* L, int Idx)
{
    int32 Type = lua_type(L, Idx);
    switch (Type)
    {
    case LUA_TBOOLEAN:
        return "bool";
    case LUA_TNUMBER:
        return lua_isinteger(L, Idx) > 0 ? "int" : "float";
    case LUA_TSTRING:
    {
        const char* Name = lua_tostring(L, Idx);
        if (Name == NULL || Name[0] == 0)
            return "FString";
        return Name;
    }
    case LUA_TTABLE:
    {
        lua_pushstring(L, "__name");
        Type = lua_rawget(L, Idx);
        if (Type == LUA_TSTRING)
        {
            const char* Name = lua_tostring(L, -1);
            lua_pop(L, 1);
            return Name;
        }
        lua_pop(L, 1);
    }
    case LUA_TUSERDATA:
    {
        FCObjRef *ObjRef = (FCObjRef *)FCScript::GetObjRefPtr(L, Idx);
        if(ObjRef && ObjRef->ClassDesc)
        {
            return ObjRef->ClassDesc->m_UEClassName;
        }
        break;
    }
    break;
    default:
        break;
    }
    return "FString";   
}

int ScriptStruct_New(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));
    // 结构体的话，可以由lua自己维护
    int64 ObjID = FCGetObj::GetIns()->PushNewStruct(ClassDesc);
    FCScript::PushBindObjRef(L, ObjID, ClassDesc->m_UEClassName);
    return 1;
}
int Class_StaticClass(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));
    UClass* Class = ClassDesc->m_Class;

    LuaPushValue(L, Class, false);
    return 1;
}
int Class_Cast(lua_State* L)
{
    UObject* Object = FCScript::GetUObject(L, 1);
    UClass* Class = Cast<UClass>(FCScript::GetUObject(L, 2));
    if(Class && Object)
    {
        if ((Object->IsA(Class) || (Class->HasAnyClassFlags(CLASS_Interface) && Class != UInterface::StaticClass() && Object->GetClass()->ImplementsInterface(Class))))
        {
            lua_pushvalue(L, 1);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}
int UObject_Identical(lua_State* L)
{
    UObject* A = FCScript::GetUObject(L, 1);
    UObject* B = FCScript::GetUObject(L, 2);
    lua_pushboolean(L, A == B);
    return 1;
}
int UObject_Delete(lua_State* L)
{
    int64 ObjID = FCScript::GetObjID(L, 1);
    FCGetObj::GetIns()->DeleteValue(ObjID);
    return 0;
}

// 覆写
int BindScript_Overridden(lua_State* L)
{
    int64 ObjID = (int64)lua_touserdata(L, lua_upvalueindex(1));
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(2));
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    if (ObjRef && ObjRef->IsValid())
    {
        const char* FieldName = lua_tostring(L, 2);
        FCDynamicFunction* DynamicFunction = ClassDesc->FindFunctionByName(FieldName);
        if (DynamicFunction)
        {
            lua_pushlightuserdata(L, (void*)ObjID);
            lua_pushstring(L, FieldName);
            lua_pushlightuserdata(L, (void*)DynamicFunction);
            lua_pushcclosure(L, BindScript_CallOverridenFunction, 3);        // closure
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

int   BindScript_CallOverridenFunction(lua_State* L)
{
    int64 ObjID = (int64)lua_touserdata(L, lua_upvalueindex(1));  // 从闭包中取第一个参数
    const char* FuncName = lua_tostring(L, lua_upvalueindex(2)); // 
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    int RetCount = 0;
    if (ObjRef && ObjRef->IsValid())
    {
        FCDynamicClassDesc* DynamicClassDesc = ObjRef->ClassDesc;
        FCDynamicFunction* DynamicFunc = DynamicClassDesc->FindFunctionByName(FuncName);
        UObject* Object = ObjRef->GetUObject();
        if (!Object && ObjRef->Parent)
            Object = ObjRef->Parent->GetUObject();
        if (DynamicFunc && Object)
        {
            RetCount = FFCObjectdManager::GetSingleIns()->NativeCall(Object, DynamicFunc, L, 2);
        }
    }
    return RetCount;
}

// 属性设置走NewIndex, Object.value = xxx
// 对不存在的索引(成员变量)赋值时触发
int Class_NewIndex(lua_State* L)
{
    Global_SetUProperty(L);
    return 0;
}

// 注册枚举(无返回值的函数)
int Global_RegisterEnum(lua_State* L)
{
    int32 NumParams = lua_gettop(L);
    if (NumParams < 1)
    {
        return 0;
    }
    const char* ClassName = lua_tostring(L, 1);
    GlbRegisterEnum(L, ClassName);
    return 0;
}

void GlbRegisterEnum(lua_State* L, const char* ClassName)
{
    const FDynamicEnum* EnumClass = GetScriptContext()->RegisterEnum(ClassName);
    if(!EnumClass)
        return ;
    int32 Type = luaL_getmetatable(L, ClassName);
    if (Type != LUA_TTABLE)
    {
        luaL_newmetatable(L, ClassName);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, Enum_Index);
        lua_rawset(L, -3);

        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, Enum_Delete);
        lua_rawset(L, -3);

        // add other members here

        lua_pushvalue(L, -1);               // set metatable to self
        lua_setmetatable(L, -2);

        SetTableForClass(L, ClassName);
    }
    lua_pop(L, 1);
}

bool GlbReigterClassEx(lua_State* L, FCDynamicClassDesc* ClassDesc, const char* ClassName)
{
    int32 Type = luaL_getmetatable(L, ClassName);
    lua_pop(L, 1);
    if (Type == LUA_TTABLE)
    {
        return true;
    }
    if(!ClassDesc)
    {
        ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
    }

    if (!ClassDesc)
    {
        // 为免存泄露，也需要创建一个
        luaL_newmetatable(L, ClassName);                // 1, will be used as meta table later (lua_setmetatable)

        lua_pushstring(L, "__gc");                      // Key
        lua_pushcfunction(L, UObject_Delete);           // C function
        lua_rawset(L, -3);

        lua_pushvalue(L, -1);                           // set metatable to self
        lua_setmetatable(L, -2);

        SetTableForClass(L, ClassName);

        return false;
    }
    luaL_newmetatable(L, ClassName);                  // 1, will be used as meta table later (lua_setmetatable)

    if (ClassDesc->m_Super)
    {
        const char* InSuperClassName = ClassDesc->m_SuperName;
        lua_pushstring(L, "ParentClass");                   // 2
        Type = luaL_getmetatable(L, InSuperClassName);
        if (Type != LUA_TTABLE)
        {
            printf("Invalid super class, className:%s, SupperClassName:%s", ClassName, InSuperClassName);
        }
        lua_rawset(L, -3);
    }

    lua_pushstring(L, "__index");                           // 2  对不存在的索引(成员变量)访问时触发
    lua_pushcfunction(L, Class_Index);                      // 3
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");                        // 2  对不存在的索引(成员变量)赋值时触发
    lua_pushcfunction(L, Class_NewIndex);                   // 3
    lua_rawset(L, -3);

    UScriptStruct* ScriptStruct = ClassDesc->m_ScriptStruct;
    if (ScriptStruct)
    {
        lua_pushlightuserdata(L, ClassDesc);                // FClassDesc这里写入table的C类型

        //lua_pushstring(L, "Copy");                          // Key
        //lua_pushvalue(L, -2);                               // FClassDesc
        //lua_pushcclosure(L, ScriptStruct_Copy, 1);          // closure
        //lua_rawset(L, -4);

        //lua_pushstring(L, "CopyFrom");
        //lua_pushvalue(L, -2);
        //lua_pushcclosure(L, ScriptStruct_CopyFrom, 1);
        //lua_rawset(L, -4);

        lua_pushstring(L, "__eq");                          // Key
        lua_pushvalue(L, -2);                               // FClassDesc
        lua_pushcclosure(L, ScriptStruct_Compare, 1);       // closure
        lua_rawset(L, -4);

        //if (!(ScriptStruct->StructFlags & (STRUCT_IsPlainOldData | STRUCT_NoDestructor)))
        {
            lua_pushstring(L, "__gc");                      // Key
            lua_pushvalue(L, -2);                           // FClassDesc
            lua_pushcclosure(L, ScriptStruct_Delete, 1);    // closure
            lua_rawset(L, -4);
        }

        lua_pushstring(L, "__call");                        // Key
        lua_pushvalue(L, -2);                               // FClassDesc
        lua_pushcclosure(L, ScriptStruct_New, 1);           // closure
        lua_rawset(L, -4);

        lua_pop(L, 1);
    }
    else
    {
        UClass* Class = ClassDesc->m_Class;
        if (Class != UObject::StaticClass() && Class != UClass::StaticClass())
        {
            //lua_pushstring(L, "ClassDesc");                 // Key
            //lua_pushlightuserdata(L, ClassDesc);            // FClassDesc
            //lua_rawset(L, -3);

            //lua_pushstring(L, "StaticClass");               // Key
            //lua_pushlightuserdata(L, ClassDesc);            // FClassDesc
            //lua_pushcclosure(L, Class_StaticClass, 1);      // closure
            //lua_rawset(L, -3);

            //lua_pushstring(L, "Cast");                      // Key
            //lua_pushcfunction(L, Class_Cast);               // C function
            //lua_rawset(L, -3);

            lua_pushstring(L, "__eq");                      // Key
            lua_pushcfunction(L, UObject_Identical);        // C function
            lua_rawset(L, -3);

            lua_pushstring(L, "__gc");                      // Key
            lua_pushcfunction(L, UObject_Delete);           // C function
            lua_rawset(L, -3);
        }
        else
        {
            lua_pushstring(L, "__gc");                      // Key
            lua_pushcfunction(L, UObject_Delete);           // C function
            lua_rawset(L, -3);
        }
    }
    ClassDesc->OnLibOpen(L);

    lua_pushvalue(L, -1);                                   // set metatable to self
    lua_setmetatable(L, -2);

    SetTableForClass(L, ClassName);
    return true;
}

bool GlbRegisterBindScriptMetatable(lua_State* L, FCDynamicClassDesc* ClassDesc, const char* ClassName)
{
    int32 Type = luaL_getmetatable(L, ClassName);
    lua_pop(L, 1);
    if (Type == LUA_TTABLE)
    {
        return true;
    }
    luaL_newmetatable(L, ClassName);                  // 1, will be used as meta table later (lua_setmetatable)
    if (ClassDesc->m_Super)
    {
        const char* InSuperClassName = ClassDesc->m_SuperName;
        lua_pushstring(L, "ParentClass");                   // 2
        Type = luaL_getmetatable(L, InSuperClassName);
        if (Type != LUA_TTABLE)
        {
            printf("Invalid super class, className:%s, SupperClassName:%s", ClassName, InSuperClassName);
        }
        lua_rawset(L, -3);
    }

    lua_pushstring(L, "__index");                           // 2  对不存在的索引(成员变量)访问时触发
    lua_pushcfunction(L, OtherScript_Index);                      // 3
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");                        // 2  对不存在的索引(成员变量)赋值时触发
    lua_pushcfunction(L, OtherScript_NewIndex);                   // 3
    lua_rawset(L, -3);

    lua_pushstring(L, "__eq");                      // Key
    lua_pushcfunction(L, UObject_Identical);        // C function
    lua_rawset(L, -3);

    lua_pushstring(L, "__gc");                      // Key
    lua_pushcfunction(L, UObject_Delete);           // C function
    lua_rawset(L, -3);

    ClassDesc->OnLibOpen(L);

    lua_pushvalue(L, -1);                                   // set metatable to self
    lua_setmetatable(L, -2);

    SetTableForClass(L, ClassName);
    return true;
}

bool  GlbRegisterClass(lua_State* L, const char* ClassName)
{
    return GlbReigterClassEx(L, nullptr, ClassName);
}

// 注册元表类(无返回值的函数)
int Global_RegisterClass(lua_State* L)
{
    int32 NumParams = lua_gettop(L);
    if (NumParams < 1)
    {
        return 0;
    }
    const char* ClassName = lua_tostring(L, 1);
    GlbRegisterClass(L, ClassName);
    return 0;
}
//---------------------------------------------------------------------------------

// 反射类的对象属性读取
int Global_GetUProperty(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef *)FCScript::GetObjRefPtr(L, 1);
    const char* FieldName = lua_tostring(L, 2); // value
    if (ObjRef && ObjRef->IsValid())
    {
        FCDynamicClassDesc* ClassDesc = ObjRef->ClassDesc;
        FCDynamicField* Field = ClassDesc->FindFieldByName(FieldName);
        if (Field)
        {
            return Field->DoGet(L, ObjRef, ClassDesc);
        }
    }
    const char* ClassName = GetUETypeNameFromLua(L, 1);
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
    if (ClassDesc)
    {
        FCDynamicField* Field = ClassDesc->FindFieldByName(FieldName);
        if (Field)
        {
            return Field->DoGet(L, ObjRef, ClassDesc);
        }
    }

    ReportLuaError(L, "invalid property, failed to call GetUProperty");
    // 查找函数
    lua_pushnil(L);
    return 1;
}

// 设置原生的table变量
// (table, key, value)
void  Global_SetTableValue(lua_State* L)
{
    int32 Type = lua_type(L, 1);
    if (Type == LUA_TTABLE)
    {
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_rawset(L, 1);
        //printf("You are modifying metatable! Please make sure you know what you are doing!");
    }
}

// 反射类的对象属性设置
// (table, key, value)
int Global_SetUProperty(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->IsValid())
    {
        const char* FieldName = lua_tostring(L, 2); // value
        FCDynamicClassDesc* ClassDesc = ObjRef->ClassDesc;
        FCDynamicField* Field = ClassDesc->FindFieldByName(FieldName);
        if (Field)
        {
            return Field->DoSet(L, ObjRef, ClassDesc);
        }
    }
    ReportLuaError(L, "invalid property, failed to call SetUProperty");
    //const char* ClassName = GetUETypeNameFromLua(L, 1);
    //FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(ClassName);
    //if (ClassDesc)
    //{
    //    const char* FieldName = lua_tostring(L, 2); // value
    //    FCDynamicField* Field = ClassDesc->FindFieldByName(FieldName);
    //    if (Field)
    //    {
    //        //return Field->DoSet(L, ObjRef, ClassDesc);  // 不支持全局变量的写入了
    //    }
    //}
    ////Global_SetTableValue(L);
    return 0;    
}

int Global_Index(lua_State* L)
{
    int Type = lua_type(L, 2);
    if(Type == LUA_TSTRING)
    {
        const char* FieldName = lua_tostring(L, 2); // value
        if(FieldName)
        {
            char ch = FieldName[0];
            if(ch == 'U' || ch == 'A' || ch == 'F')
            {
                FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass(FieldName);
                if(ClassDesc)
                {
                    GlbReigterClassEx(L, ClassDesc, FieldName);
                }
            }
            else if(ch == 'E')
            {
                GlbRegisterEnum(L, FieldName);
            }
        }
    }
    lua_rawget(L, -2);
    return 1;
}

int BindScript_GetBPObject(lua_State* L)
{
    // self:GetObject()
    int64 ObjID = (int64)lua_touserdata(L, lua_upvalueindex(1));
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    if (ObjRef && ObjRef->IsValid())
    {
        ObjRef->Ref++;
        FCScript::PushBindObjRef(L, ObjID, ObjRef->ClassDesc->m_UEClassName);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int DoScript_Index(lua_State* L, FCObjRef* ObjRef)
{
    const char* FieldName = lua_tostring(L, 2);
    if (ObjRef && ObjRef->IsValid())
    {
        FCDynamicClassDesc* ClassDesc = ObjRef->ClassDesc;
        FCDynamicField* Field = ClassDesc->FindFieldByName(FieldName);
        if (Field)
        {
            return Field->DoGet(L, ObjRef, ClassDesc);
        }
    }
    int Ret = lua_rawget(L, 1);
    int Type = lua_type(L, -1);
    if (Type > LUA_TNIL)
    {
        return 1;
    }

    lua_pushvalue(L, 1);
    while (true)
    {
        lua_pushstring(L, "Super");
        lua_rawget(L, -2);
        Type = lua_type(L, -1);
        if (Type == LUA_TTABLE)
        {
            lua_getfield(L, -1, FieldName);
            int ChildType = lua_type(L, -1);
            if (ChildType > LUA_TNIL)
            {
                //lua_remove(L, -2);
                return 1;
            }
            lua_pop(L, 1);
        }
        else
        {
            break;
        }
    }
    lua_pushnil(L);
    return 1;
}

int BindScript_Index(lua_State* L)
{
    // (table, key)
    int64 ObjID = (int64)lua_touserdata(L, lua_upvalueindex(1));
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    return DoScript_Index(L, ObjRef);
}

int OtherScript_Index(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    return DoScript_Index(L, ObjRef);
}

int DoScript_NewIndex(lua_State* L, FCObjRef* ObjRef)
{
    const char* FieldName = lua_tostring(L, 2);
    if (ObjRef && ObjRef->IsValid())
    {
        FCDynamicClassDesc* ClassDesc = ObjRef->ClassDesc;
        FCDynamicField* Field = ClassDesc->FindFieldByName(FieldName);
        if (Field)
        {
            return Field->DoSet(L, ObjRef, ClassDesc);
        }
    }
    lua_rawset(L, 1);
    return 0;
}

int BindScript_NewIndex(lua_State* L)
{
    int64 ObjID = (int64)lua_touserdata(L, lua_upvalueindex(1));
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    return DoScript_NewIndex(L, ObjRef);
}

int OtherScript_NewIndex(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    return DoScript_NewIndex(L, ObjRef);
}

int BindScript_Equal(lua_State* L)
{
    UObject* A = FCScript::GetUObject(L, 1);
    UObject* B = FCScript::GetUObject(L, 2);
    lua_pushboolean(L, A == B);
    return 1;
}

int WrapNativeCallFunction(lua_State* L, int ParamIndex, UObject *ThisObject, FCDynamicFunction  *DynamicFunc, uint8 *Buffer, int BufferSize, FNativeFuncPtr NativeFuncPtr)
{
    int  StackSize = DynamicFunc->ParmsSize;
    UFunction* Function = DynamicFunc->Function;
    int nParamCount = DynamicFunc->ParamCount;
    FCDynamicFunctionParam* BeginProperty = DynamicFunc->m_Property.data();
    FCDynamicFunctionParam* EndProperty = BeginProperty + nParamCount;
    FCDynamicFunctionParam* DynamicProperty = BeginProperty;
    uint8* Frame = Buffer;
    int nAllBuffSize = Function->PropertiesSize + Function->ParmsSize;
    //int OuterAddrOffset = nAllBuffSize;
    //int OutParmRecOffset = OuterAddrOffset + DynamicFunc->OuterParamSize;
    int OutParmRecOffset = nAllBuffSize;
    int OuterIndesOffset = OutParmRecOffset;
    if(DynamicFunc->bOuter)
    {
        //nAllBuffSize += DynamicFunc->OuterParamSize + sizeof(FOutParmRec) * DynamicFunc->OuterParamCount;
        nAllBuffSize += sizeof(FOutParmRec) * DynamicFunc->OuterParamCount;
        OuterIndesOffset = nAllBuffSize;
        nAllBuffSize += sizeof(short) * DynamicFunc->OuterParamCount;
    }
    if (nAllBuffSize > BufferSize)
    {
        nAllBuffSize = (nAllBuffSize + 15) / 16 * 16;
        Frame = (uint8*)FMemory_Alloca(nAllBuffSize);
        //Frame = (uint8*)FMemory::Malloc(nAllBuffSize, 16);
    }
    FMemory::Memzero(Frame, nAllBuffSize);

    int NumParams = lua_gettop(L) + 1;

    int Index = ParamIndex;
    uint8  *Locals = Frame;
    uint8  *ValueAddr = Locals;
    //uint8  *OuterAddr = Locals + OuterAddrOffset;
    FOutParmRec* FristOuterParms = (FOutParmRec*)(Locals + OutParmRecOffset);
    FOutParmRec* OuterParms = FristOuterParms;
    int LatentPropertyIndex = DynamicFunc->LatentPropertyIndex;
    short *OuterIndexs = (short *)(Locals + OuterIndesOffset);
    // 将脚本参数传给UE的函数
    for (; DynamicProperty < EndProperty; ++DynamicProperty, ++Index)
    {
        ValueAddr = Locals + DynamicProperty->Offset_Internal;
        DynamicProperty->SafePropertyPtr->InitializeValue(ValueAddr);
        if(LatentPropertyIndex == DynamicProperty->PropertyIndex)
        {
            int32 ThreadRef = GetScriptContext()->QueryLuaRef(L);
            FLatentActionInfo LatentActionInfo(ThreadRef, GetTypeHash(FGuid::NewGuid()), TEXT("OnLatentActionCompleted"), GetScriptContext()->m_Ticker);
            DynamicProperty->SafePropertyPtr->CopySingleValue(ValueAddr, &LatentActionInfo);
        }
        DynamicProperty->bTempRealRef = false;
        if(Index < NumParams)
        {
            DynamicProperty->bTempNeedRef = true;
            DynamicProperty->m_ReadScriptFunc(L, Index, DynamicProperty, ValueAddr, nullptr, nullptr);
            DynamicProperty->bTempNeedRef = false;
        }
        else
        {
            // 如果有默认参数，就使用默认参数
            if(DynamicProperty->DefaultParam)
                DynamicProperty->DefaultParam->WriteDefaultValue(DynamicProperty, ValueAddr);
        }

        if(DynamicProperty->bOuter)
        {
            *OuterIndexs++ = (short)(Index - ParamIndex);
            //DynamicProperty->SafePropertyPtr->InitializeValue(OuterAddr);
            //OuterParms->PropAddr = OuterAddr;
            //OuterAddr += DynamicProperty->SafePropertyPtr->ElementSize;

            OuterParms->PropAddr = ValueAddr;
            OuterParms->NextOutParm = nullptr;
            OuterParms->Property = (FProperty *)DynamicProperty->SafePropertyPtr->GetProperty();
            if(OuterParms != FristOuterParms)
            {
                (OuterParms-1)->NextOutParm = OuterParms;
            }
            ++OuterParms;
        }
    }

    if (DynamicFunc->ReturnPropertyIndex >= 0)
    {
        DynamicProperty = BeginProperty + DynamicFunc->ReturnPropertyIndex;
        ValueAddr = Locals + DynamicProperty->Offset_Internal;
        DynamicProperty->SafePropertyPtr->InitializeValue(ValueAddr);
    }

    FFrame NewStack(ThisObject, Function, Frame, NULL, GetChildProperties(Function));
    const bool bHasReturnParam = Function->ReturnValueOffset != MAX_uint16;
    uint8* ReturnValueAddress = bHasReturnParam ? ((uint8*)Frame + Function->ReturnValueOffset) : nullptr;
    if(DynamicFunc->bOuter && FristOuterParms->Property)
    {
        NewStack.OutParms = FristOuterParms;
    }
    else
    {
        FristOuterParms = nullptr;
    }
    if(NativeFuncPtr)
    {
        NativeFuncPtr(ThisObject, NewStack, ReturnValueAddress);
    }
    else
    {
        //Function->Invoke(ThisObject, NewStack, ReturnValueAddress);
        if(ThisObject)
        {
            //ThisObject->UObject::ProcessEvent(Function, Frame);
            Function->Invoke(ThisObject, NewStack, ReturnValueAddress);
        }
        else
        {
            NativeFuncPtr = Function->GetNativeFunc();
            if(NativeFuncPtr)
                NativeFuncPtr(ThisObject, NewStack, ReturnValueAddress);
        }
    }

    //ThisObject->UObject::ProcessEvent(Function, Frame);

    // 将返回值传给脚本
    int RetCount = 0;
    if (DynamicFunc->ReturnPropertyIndex >= 0)
    {
        DynamicProperty = BeginProperty + DynamicFunc->ReturnPropertyIndex;
        ValueAddr = Locals + DynamicProperty->Offset_Internal;
        DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr);
        DynamicProperty->SafePropertyPtr->DestroyValue(ValueAddr);
        ++RetCount;
    }

    // 拷贝返回给脚本的变量, lua不支持修改栈上输入的参数, 只能通过多个参数返回
    OuterParms = FristOuterParms;
    OuterIndexs = (short*)(Locals + OuterIndesOffset);
    while(OuterParms)
    {
        ValueAddr = OuterParms->PropAddr;
        Index = *OuterIndexs++;
        DynamicProperty = BeginProperty + Index;
        if (!DynamicProperty->SafePropertyPtr->HasAnyPropertyFlags(CPF_ConstParm))
        {
            if(!DynamicProperty->m_CopyScriptValue(L, ParamIndex + Index, DynamicProperty, ValueAddr, nullptr, nullptr)) // 尝试修改栈上的变量
            {
                DynamicProperty->m_WriteScriptFunc(L, DynamicProperty, ValueAddr, nullptr, nullptr); // 返回多个值
            }
            ++RetCount;
        }
        //DynamicProperty->Property->DestroyValue(ValueAddr);
        OuterParms = OuterParms->NextOutParm;
    }

    // 释放临时变量
    DynamicProperty = BeginProperty;
    for (; DynamicProperty < EndProperty; ++DynamicProperty)
    {
        ValueAddr = Locals + DynamicProperty->Offset_Internal;
        if(!DynamicProperty->bTempRealRef)
            DynamicProperty->SafePropertyPtr->DestroyValue(ValueAddr);
        DynamicProperty->bTempRealRef = false;
    }

    // 释放临时内存
    if (Frame != Buffer)
    {
        //FMemory::Free(Frame);
    }
    return RetCount;
}

// 普通的函数调用
int   Class_CallFunction(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));  // 从闭包中取第一个参数
    int64 HostObjID = (int64)lua_touserdata(L, lua_upvalueindex(2));  // 从闭包中取第二个参数
    FCDynamicFunction *DynamicFunc = (FCDynamicFunction *)lua_touserdata(L, lua_upvalueindex(3));  // 从闭包中取第三个参数
    FCObjRef* HostRef = FCGetObj::GetIns()->FindValue(HostObjID);
    int RetCount = 0;
    if(DynamicFunc)
    {
        UObject* ThisObject = nullptr;
        if(HostRef && HostRef->IsValid())
        {
            ThisObject = HostRef->GetUObject();
            if (!ThisObject && HostRef->Parent)
                ThisObject = HostRef->Parent->GetUObject();
        }
        if (!ThisObject)
        {
            if (FUNC_Static != (FUNC_Static & DynamicFunc->Function->FunctionFlags))
            {
                ReportLuaError(L, "invalid object, failed call function");
                return 0;
            }
        }
        int StartIdx = (FUNC_Static & DynamicFunc->Function->FunctionFlags) ? 1 : 2;
        uint8   Buffer[256];
        RetCount = WrapNativeCallFunction(L, StartIdx, ThisObject, DynamicFunc, Buffer, sizeof(Buffer), nullptr);
    }
    return RetCount;
}

int  Class_CallLibFunction(lua_State* L, bool bGet)
{
    FCDynamicWrapLibFunction * DynamicFunc = (FCDynamicWrapLibFunction *)lua_touserdata(L, lua_upvalueindex(1));  // 从闭包中取第一个参数
    int64 HostObjID = (int64)lua_touserdata(L, lua_upvalueindex(2));  // 从闭包中取第二个参数
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(3));  // 从闭包中取第三个参数
    FCObjRef* HostRef = FCGetObj::GetIns()->FindValue(HostObjID);
    int RetCount = 0;
    if (DynamicFunc)
    {
        UObject* ThisObject = nullptr;
        if (HostRef && HostRef->IsValid())
        {
            ThisObject = HostRef->GetUObject();
            if (!ThisObject && HostRef->Parent)
            {
                ThisObject = HostRef->Parent->GetUObject();
            }
        }
        LPWrapLibFunction  LibFunc = bGet ? DynamicFunc->m_GetFunction : DynamicFunc->m_SetFunction;
        if (LibFunc)
        {
            return LibFunc(L, HostRef, ThisObject);
        }
    }
    return 0;
}

int  Class_CallInterfaceFunction(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = (FCDynamicClassDesc*)lua_touserdata(L, lua_upvalueindex(1));  // 从闭包中取第一个参数
    int64 HostObjID = (int64)lua_touserdata(L, lua_upvalueindex(2));  // 从闭包中取第二个参数
    FCDynamicFunction* InterfaceFunc = (FCDynamicFunction*)lua_touserdata(L, lua_upvalueindex(3));  // 从闭包中取第三个参数
    FCObjRef* HostRef = FCGetObj::GetIns()->FindValue(HostObjID);
    int RetCount = 0;

    FCObjRef* ArgObjectRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);    
    // UInterface.AnyFunc(Object, param ... )
    if(ArgObjectRef)
    {
        UObject* ThisObject = ArgObjectRef->GetUObject();
        if(ThisObject)
        {
            FCDynamicClassDesc *DynamicClassDesc = GetScriptContext()->RegisterUStruct(ThisObject->GetClass());
            FCDynamicField *FuncField = DynamicClassDesc->FindFieldByName(InterfaceFunc->FieldName);
            if(FuncField && FuncField->IsDynamicFunction())
            {
                FCDynamicFunction *ThisDynamicFunc = (FCDynamicFunction*)FuncField;
                int StartIdx = 2;
                uint8   Buffer[256];
                RetCount = WrapNativeCallFunction(L, StartIdx, ThisObject, ThisDynamicFunc, Buffer, sizeof(Buffer), nullptr);
                return RetCount;
            }
            // 没有的话，就直接调用对应的脚本函数吧
            int64 ScriptIns = -1;
            if (FFCObjectdManager::GetSingleIns()->GetBindScriptIns(ArgObjectRef->PtrIndex, ScriptIns))
            {
                int ParamsCount = lua_gettop(L) - 1;
                int ParamStartIdx = 2;
                RetCount = ScriptCallInterface(L, ScriptIns, InterfaceFunc->FieldName, ParamStartIdx, ParamsCount);
                return RetCount;
            }
        }
    }

    return 0;
}

int   Class_CallGetLibFunction(lua_State* L)
{
    return Class_CallLibFunction(L, true);
}

int   Class_CallSetLibFunction(lua_State* L)
{
    return Class_CallLibFunction(L, false);
}

// sleep这类的，需要将当前脚本挂起
int   Class_CallLatentFunction(lua_State* L)
{
    int RetCount = Class_CallFunction(L);
    return lua_yield(L, RetCount);
}

int Global_WaitFunction(lua_State* L)
{
    // Wait()
    float  DelayTime = (float)lua_tonumber(L, 1);
    int32 ThreadRef = GetScriptContext()->QueryLuaRef(L);

    FDynamicWaitInfo  Info;
    Info.ThreadRef = ThreadRef;
    Info.WaitTime = DelayTime;

    GetScriptContext()->m_WaitList.push_back(Info);

    return lua_yield(L, 0);
}

void   RunTimeRegisterScript(FCScriptContext *Context)
{
    lua_State  *L = Context->m_LuaState;

    lua_register(L, "RegisterEnum", Global_RegisterEnum);
    lua_register(L, "RegisterClass", Global_RegisterClass);
    lua_register(L, "Global_Index", Global_Index);
    lua_register(L, "GetUProperty", Global_GetUProperty);
    lua_register(L, "SetUProperty", Global_SetUProperty);
    lua_register(L, "print", Global_Print);
    lua_register(L, "UEPrint", Global_Print);
    lua_register(L, "require", Global_Require);
    lua_register(L, "GlbWait", Global_WaitFunction);

    //lua_gc(L, LUA_GCSETPAUSE, 100);
    //lua_gc(L, LUA_GCSETSTEPMUL, 5000);
}
