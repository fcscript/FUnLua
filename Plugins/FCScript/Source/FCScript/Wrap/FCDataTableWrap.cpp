#include "FCDataTableWrap.h"
#include "FCGetObj.h"
#include "FCTemplateType.h"
#include "FCUEUtilWrap.h"
#include "FCRunTimeRegister.h"

#include "Kismet/DataTableFunctionLibrary.h"

void FCDataTableWrap::Register(lua_State* L)
{
    FCDynamicClassDesc* ClassDesc = GetScriptContext()->RegisterUClass("UDataTable");
    if (ClassDesc)
    {
        ClassDesc->RegisterWrapLibFunction("Load", DoLoad_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetRow", DoGetRow_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetRowNumb", DoGetRowNumb_wrap, nullptr);
        ClassDesc->RegisterWrapLibFunction("GetRowDataStructure", DoGetRow_wrap, nullptr);

        ClassDesc->AddSystemFunction("__pairs", obj_pairs);
        ClassDesc->AddSystemFunction("__index", obj_Index);
        ClassDesc->AddSystemFunction("__len", GetNumb_Wrap);
    }
}

// 
// UDataTable.Load("/Game/TestDataTable.TestDataTable")
int FCDataTableWrap::DoLoad_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    return FCUEUtilWrap::LoadObject_wrap(L);
}

// dataTable->Load(RowName)
// UStruct *UDataTable::GetRow(FString RowName);
int FCDataTableWrap::DoGetRow_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    UObject* Arg1 = FCScript::GetUObject(L, 1);
    UDataTable *DataTable = Cast<UDataTable>(Arg1);
    const char *Arg2 = lua_tostring(L, 2);
    if(DataTable && Arg2)
    {
        FName  RowName(Arg2);
        void* RowPtr = DataTable->FindRowUnchecked(RowName);
        UScriptStruct* RowStruct = (UScriptStruct *)DataTable->GetRowStruct();
        if(RowStruct && RowPtr)
        {
            FCDynamicProperty* DynamicProperty = GetStructDynamicProperty(RowStruct);

            int64 ObjID = FCGetObj::GetIns()->PushProperty(DataTable, DynamicProperty, RowPtr);
            FCScript::PushBindObjRef(L, ObjID, DynamicProperty->ClassName);
            return 1;
        }
        else
        {
            lua_pushnil(L);
            return 1;
        }
    }
    return 0;
}

int FCDataTableWrap::DoGetRowNumb_wrap(lua_State* L, void* ObjRefPtr, UObject* ThisObject)
{
    return GetNumb_Wrap(L);
}

struct FCDataTablePair
{
    UDataTable* DataTable;
    TMap<FName, uint8*>::TConstIterator PairIt;
    FCDataTablePair(UDataTable* InDataTable, const TMap<FName, uint8*>::TConstIterator &InIt):DataTable(InDataTable), PairIt(InIt){}
};

// key(row name), value(data table)

int FCDataTableWrap::pair_gc(lua_State* L)
{
    FCDataTablePair *Pair = (FCDataTablePair *)lua_touserdata(L, 1);
    if(Pair)
    {
        Pair->~FCDataTablePair();
    }
    return 0;
}

int FCDataTableWrap::obj_NextPairs(lua_State* L)
{
    FCDataTablePair* Pair = (FCDataTablePair*)lua_touserdata(L, 1);
    if(Pair && Pair->DataTable)
    {
        const TMap<FName, uint8*>& RowMap = Pair->DataTable->GetRowMap();
        if(Pair->PairIt)
        {
            FCScript::SetArgValue(L, Pair->PairIt.Key());
            uint8 *ValueAddr = Pair->PairIt.Value();

            UScriptStruct* RowStruct = (UScriptStruct*)Pair->DataTable->GetRowStruct();
            FCDynamicProperty* DynamicProperty = GetStructDynamicProperty(RowStruct);
            int64 ObjID = FCGetObj::GetIns()->PushProperty(Pair->DataTable, DynamicProperty, ValueAddr);
            FCScript::PushBindObjRef(L, ObjID, DynamicProperty->ClassName);

            ++(Pair->PairIt);
            return 2;
        }
    }
    return 0;
}

int FCDataTableWrap::obj_pairs(lua_State* L)
{
    UObject* Arg1 = FCScript::GetUObject(L, 1);
    UDataTable* DataTable = Cast<UDataTable>(Arg1);
    if(!DataTable)
    {
        return 0;
    }
    lua_pushcfunction(L, obj_NextPairs);
    FCDataTablePair* Pairs = (FCDataTablePair*)lua_newuserdata(L, sizeof(FCDataTablePair));
    const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
    Pairs = new (Pairs)FCDataTablePair(DataTable, RowMap.CreateConstIterator());

    lua_newtable(L);
    lua_pushcfunction(L, pair_gc);
    lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushnil(L);

    return 3;
}

int FCDataTableWrap::obj_Index(lua_State* L)
{
    UObject* Arg1 = FCScript::GetUObject(L, 1);
    UDataTable* DataTable = Cast<UDataTable>(Arg1);
    int Type = lua_type(L, 2);
    if (Type == LUA_TSTRING && DataTable)
    {
        const char *Arg2 = lua_tostring(L, 2);
        if(Arg2)
        {
            FName  RowName(Arg2);
            void* RowPtr = DataTable->FindRowUnchecked(RowName);
            UScriptStruct* RowStruct = (UScriptStruct*)DataTable->GetRowStruct();
            if (RowStruct && RowPtr)
            {
                FCDynamicProperty* DynamicProperty = GetStructDynamicProperty(RowStruct);

                int64 ObjID = FCGetObj::GetIns()->PushProperty(DataTable, DynamicProperty, RowPtr);
                FCScript::PushBindObjRef(L, ObjID, DynamicProperty->ClassName);
                return 1;
            }
        }
    }

    return Class_Index(L);
}

int FCDataTableWrap::GetNumb_Wrap(lua_State* L)
{
    UObject* Arg1 = FCScript::GetUObject(L, 1);
    UDataTable* DataTable = Cast<UDataTable>(Arg1);
    if (DataTable)
    {
        int Num = DataTable->GetRowMap().Num();
        lua_pushinteger(L, Num);
    }
    else
    {
        lua_pushinteger(L, 0);
    }
    return 1;
}
