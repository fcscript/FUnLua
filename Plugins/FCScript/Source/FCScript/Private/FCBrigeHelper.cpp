#include "FCBrigeHelper.h"
#include "FCGetObj.h"
#include "../LuaCore/LuaContext.h"

typedef std::unordered_map<const char*, FCExportedClass*, FCStringHash, FCStringEqual>   CExportClassNameMap;
FCExportedClass* FCExportedClass::s_pExportedIns = nullptr;
CExportClassNameMap  sExportClassMap;

char* GetExportClassIns(int64 ObjID)
{
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    if (ObjRef && ObjRef->RefType == EFCObjRefType::CppPtr)
    {
        return (char*)ObjRef->GetThisAddr();
    }
    return nullptr;
}

FCExportedClass* FCExportedClass::FindExportedClass(const char* InClassName)
{
	FCExportedClass  *ClassPtr = s_pExportedIns;
	while(ClassPtr)
	{
		if(ClassPtr->ClassName == InClassName || strcmp(ClassPtr->ClassName, InClassName) == 0)
		{
			return ClassPtr;
		}
		ClassPtr = ClassPtr->NextClass;
	}
	return nullptr;
}

void FCExportedClass::RegisterAll(lua_State* L)
{
	FCExportedClass* ClassPtr = s_pExportedIns;
	while (ClassPtr)
	{
		ClassPtr->Register(L);
		ClassPtr = ClassPtr->NextClass;
	}
}

void FCExportedClass::UnRegisterAll(lua_State* L)
{
	FCExportedClass* ClassPtr = s_pExportedIns;
	while (ClassPtr)
	{
		ClassPtr->UnRegister(L);
		ClassPtr = ClassPtr->NextClass;
	}
}

int FCExportedClass::RegisterLibClass(lua_State* L, const char* InClassName, const LuaRegFunc* Funcs)
{
    int32 Type = luaL_getmetatable(L, InClassName);
    lua_pop(L, 1);
    if (Type == LUA_TTABLE)
    {
        return 0;
    }
    luaL_newmetatable(L, InClassName);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushvalue(L, -2);
    lua_rawset(L, -3);

    while(Funcs && Funcs->Func != nullptr)
    {
        lua_pushstring(L, Funcs->Name);
        lua_pushcfunction(L, Funcs->Func);
        lua_rawset(L, -3);
        ++Funcs;
    }

    lua_pushvalue(L, -1);                    // set metatable to self
    lua_setmetatable(L, -2);
    return 1;
}

int FCExportedClass::RegisterLibClass(lua_State* L, const char* InClassName, const LuaRegFunc* Funcs, const LuaRegAttrib* Attribs, const LuaRegFunc* TableFuncs)
{
    const LuaRegFunc *RegFunc = Funcs;
    while (RegFunc != nullptr && RegFunc->Func != nullptr)
    {
        AddLibFunction(RegFunc->Name, RegFunc->Func);
        ++RegFunc;
    }
    while (Attribs != nullptr && Attribs->Name != nullptr)
    {
        AddLibAttrib(Attribs->Name, Attribs->GetFunc, Attribs->SetFunc);
        ++Attribs;
    }
    int32 Type = luaL_getmetatable(L, InClassName);
    lua_pop(L, 1);
    if (Type == LUA_TTABLE)
    {
        return 0;
    }
    luaL_newmetatable(L, InClassName);

    while (TableFuncs->Func != nullptr && TableFuncs->Func != nullptr)
    {
        lua_pushstring(L, TableFuncs->Name);
        lua_pushcfunction(L, TableFuncs->Func);
        lua_rawset(L, -3);
        ++TableFuncs;
    }

    lua_pushvalue(L, -1);                    // set metatable to self
    lua_setmetatable(L, -2);
    return 1;
}

void* FCExportedClass::GetThisPtr(lua_State* L, const char* InClassName)
{
    int64 ObjID = (int64)lua_touserdata(L, 1);
	FCObjRef *ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
	if(ObjRef && ObjRef->RefType == EFCObjRefType::CppPtr)
	{
		return ObjRef->GetThisAddr();
	}
	return nullptr;
}

void FCExportedClass::Register(lua_State* L)
{
    if (bCustomRegistered)
        return;
	if(!Propertys && !Functions)
    {
        InitFunctionList();
    }
    sExportClassMap[ClassName] = this;

    int32 Type = luaL_getmetatable(L, ClassName);
    lua_pop(L, 1);
    if (Type == LUA_TTABLE)
    {
        return;
    }
    luaL_newmetatable(L, ClassName);             // 1, will be used as meta table later (lua_setmetatable)

    lua_pushlightuserdata(L, this);              // 这里写入table的C类型

    lua_pushstring(L, "__index");                // 2  对不存在的索引(成员变量)访问时触发
    lua_pushcclosure(L, obj_Index, 1);           // closure
    lua_rawset(L, -3);

    lua_pushstring(L, "__newindex");             // 2  对不存在的索引(成员变量)赋值时触发
    lua_pushcclosure(L, obj_NewIndex, 1);        // closure
    lua_rawset(L, -3);

    lua_pushstring(L, "__eq");                   // Key
    lua_pushcclosure(L, obj_Identical, 1);       // closure
    lua_rawset(L, -3);

    lua_pushstring(L, "__gc");                   // Key
    lua_pushcclosure(L, obj_Delete, 1);          // closure
    lua_rawset(L, -3);


    lua_pushvalue(L, -1);                                   // set metatable to self
    lua_setmetatable(L, -2);

    SetTableForClass(L, ClassName);
}

void FCExportedClass::UnRegister(lua_State* L)
{
	ReleaseList(Propertys);
	ReleaseList(Functions);
	Propertys = nullptr;
	Functions = nullptr;
    ReleaseList(InvalidItems);
}

int  FCExportedClass::DoLibIndex(lua_State* L)
{
    int64 ObjID = (int64)FCScript::GetObjID(L, 1); // table, 对象地址
    const char* FieldName = lua_tostring(L, 2); // value
    const FCExportedItem* ChildItem = GetChildItem(FieldName);
    if (ChildItem->IsFunction())
    {
        if (ChildItem->IsLibAttrib())
        {
            FCExportLibAttrib* LibAttrib = (FCExportLibAttrib*)ChildItem;
            return LibAttrib->Func(L);
        }
        else
        {
            lua_pushlightuserdata(L, this);                  // 这里写入table的C类型
            lua_pushlightuserdata(L, (void*)ObjID);                 // 这里写入table的地址
            lua_pushlightuserdata(L, (FCExportedItem*)ChildItem);   // 这里写入函数指针
            lua_pushcclosure(L, Function_wrap, 3);                  // closure
        }
        return 1;
    }
    else
    {
        char* ObjAddr = GetExportClassIns(ObjID);
        return ChildItem->Read(L, ObjAddr, this);
    }
}

int  FCExportedClass::DoLibNewIndex(lua_State* L)
{
    const char* FieldName = lua_tostring(L, 2); // value
    const FCExportedItem* ChildItem = GetChildItem(FieldName);
    if (ChildItem->IsFunction())
    {
        if (ChildItem->IsCanSet())
        {
            FCExportLibAttrib* RegFunc = (FCExportLibAttrib*)ChildItem;
            return RegFunc->SetFunc(L);
        }
        return 0;
    }
    else
    {
        int64 ObjID = (int64)lua_touserdata(L, 1); // table, 对象地址
        char* ObjAddr = GetExportClassIns(ObjID);
        return ChildItem->Write(L, ObjAddr, this);
    }
}

FCExportedItem* FCExportedClass::AddInvalidItem(const char* InChildName)
{
    FCExportInvalidItem *InvalidItem = new FCExportInvalidItem(InChildName, ClassName);
    InvalidItem->Next = InvalidItems;
    InvalidItems = InvalidItem;
    return InvalidItem;
}

void FCExportedClass::ReleaseList(FCExportedItem* InList)
{
	FCExportedItem *Item = nullptr;
	while(InList)
	{
		Item = InList;
		InList = InList->Next;
		delete Item;
	}
}

const FCExportedItem* FCExportedClass::FindChildFromList(const FCExportedItem* InListPtr, const char* InName)
{
	while (InListPtr)
	{
		if (InListPtr->Name == InName || strcmp(InListPtr->Name, InName) == 0)
		{
			return InListPtr;
		}
		InListPtr = InListPtr->Next;
	}
	return nullptr;
}

int FCExportedClass::obj_Index(lua_State* L)
{
    return GetAttrib_wrap(L);
}

int FCExportedClass::obj_NewIndex(lua_State* L)
{
    return SetAttrib_wrap(L);
}

// eq
int FCExportedClass::obj_Identical(lua_State* L)
{
    int64 ObjID1 = FCScript::GetObjID(L, 1); // table, 对象地址
    int64 ObjID2 = FCScript::GetObjID(L, 2); // table, 对象地址

    char* A = GetExportClassIns(ObjID1);
    char* B = GetExportClassIns(ObjID2);

    lua_pushboolean(L, A == B);
    return 1;
}

int FCExportedClass::obj_equal(lua_State* L)
{
    FCObjRef* ObjRef1 = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    FCObjRef* ObjRef2 = (FCObjRef*)FCScript::GetObjRefPtr(L, 2);
    lua_pushboolean(L, ObjRef1 && ObjRef2 ? ObjRef1->GetThisAddr() == ObjRef2->GetThisAddr() : false);
    return 1;
}

int FCExportedClass::obj_release(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef)
    {
        FCGetObj::GetIns()->ReleaseValue(ObjRef->PtrIndex);
    }
    return 0;
}

// gc
int FCExportedClass::obj_Delete(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef)
    {
        FCGetObj::GetIns()->DeleteValue(ObjRef->PtrIndex);
    }
    return 0;
}

// (t, key)
int FCExportedClass::GetAttrib_wrap(lua_State* L)
{
    FCExportedClass* ExportClass = (FCExportedClass*)lua_touserdata(L, lua_upvalueindex(1));
    int64 ObjID = FCScript::GetObjID(L, 1); // table, 对象地址
    const char* FieldName = lua_tostring(L, 2); // value
    const FCExportedItem* ChildItem = ExportClass->GetChildItem(FieldName);
    if(ChildItem->IsFunction())
    {
        if(ChildItem->IsLibFunction())
        {
            if (ChildItem->IsLibAttrib())
            {
                FCExportLibAttrib* RegAttrib = (FCExportLibAttrib*)ChildItem;
                return RegAttrib->Func(L);
            }
            FCExportLibFunction *RegFunc = (FCExportLibFunction*)ChildItem;
            lua_pushcfunction(L, RegFunc->Func);
        }
        else
        {
            lua_pushlightuserdata(L, ExportClass);                  // 这里写入table的C类型
            lua_pushlightuserdata(L, (void*)ObjID);                 // 这里写入table的地址
            lua_pushlightuserdata(L, (FCExportedItem*)ChildItem);   // 这里写入函数指针
            lua_pushcclosure(L, Function_wrap, 3);                  // closure
        }
        return 1;
    }
    else
    {
        char *ObjAddr = GetExportClassIns(ObjID);
        return ChildItem->Read(L, ObjAddr, ExportClass);
    }
}

int FCExportedClass::SetAttrib_wrap(lua_State* L)
{
    FCExportedClass* ExportClass = (FCExportedClass*)lua_touserdata(L, lua_upvalueindex(1));
    int64 ObjID = FCScript::GetObjID(L, 1); // table, 对象地址
    const char* FieldName = lua_tostring(L, 2); // value
    const FCExportedItem* ChildItem = ExportClass->GetChildItem(FieldName);
    if(ChildItem->IsFunction())
    {
        if (ChildItem->IsCanSet() && ChildItem->IsLibAttrib())
        {
            FCExportLibFunction* LibAttrib = (FCExportLibFunction*)ChildItem;
            return LibAttrib->Func(L);
        }
        return 0;  // 这个是非法的，函数不允许修改
    }
    else
    {
        char* ObjAddr = GetExportClassIns(ObjID);
        return ChildItem->Write(L, ObjAddr, ExportClass);
    }
}

int FCExportedClass::Function_wrap(lua_State* L)
{
    FCExportedClass* ExportClass = (FCExportedClass*)lua_touserdata(L, lua_upvalueindex(1));
    int64 ObjID = (int64)lua_touserdata(L, lua_upvalueindex(2));
    FCExportedItem* ChildItem = (FCExportedItem*)lua_touserdata(L, lua_upvalueindex(3));

    if (ChildItem->IsLibFunction())
    {
        FCExportLibFunction* LibFunc = (FCExportLibFunction*)ChildItem;
        return LibFunc->Func(L);
    }
    char* ObjAddr = GetExportClassIns(ObjID);
    return ChildItem->Invoke(L, ObjAddr, ExportClass);
}
