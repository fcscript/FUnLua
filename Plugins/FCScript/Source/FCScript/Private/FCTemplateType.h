#pragma once
#include "FCDynamicClassDesc.h"
#include "UObject/UObjectGlobals.h"
#include "Containers/ScriptArray.h"
#include "Containers/Map.h"

enum FCInnerBaseType
{
    FC_INNER_TYPE_Unknow,
    FC_INNER_TYPE_bool,            // bool变量 支持(0， 1， TRUE， FALSE, true, false)
    FC_INNER_TYPE_INT8,            // INT8
    FC_INNER_TYPE_INT16,           // INT16
    FC_INNER_TYPE_UINT16,          // UINT16
    FC_INNER_TYPE_INT,             // int
    FC_INNER_TYPE_UINT,            // uint
    FC_INNER_TYPE_FLOAT,           // float
    FC_INNER_TYPE_DOUBLE,          // double
    FC_INNER_TYPE_INT64,           // int64
    FC_INNER_TYPE_UINT64,          // uint64
    FC_INNER_TYPE_FNAME,           // 字符串
    FC_INNER_TYPE_FSTRING,         // 宽字符串        
    FC_INNER_TYPE_FTEXT,           // FText
};

#if ENGINE_MAJOR_VERSION >=5
#define ScriptArray_Add(ScriptArray, Count, ElementSize)  ScriptArray->Add(Count, ElementSize, __STDCPP_DEFAULT_NEW_ALIGNMENT__)
#define ScriptArray_Insert(ScriptArray, Index, Count, ElementSize) ScriptArray->Insert(Index, Count, ElementSize, __STDCPP_DEFAULT_NEW_ALIGNMENT__)
#define ScriptArray_Remove(ScriptArray, Index, Count, ElementSize)  ScriptArray->Remove(Index, Count, ElementSize, __STDCPP_DEFAULT_NEW_ALIGNMENT__)
#define ScriptArray_Empty(ScriptArray, Size, ElementSize)  ScriptArray->Empty(Size, ElementSize, __STDCPP_DEFAULT_NEW_ALIGNMENT__)
#else
#define ScriptArray_Add(ScriptArray, Count, ElementSize)  ScriptArray->Add(Count, ElementSize)
#define ScriptArray_Insert(ScriptArray, Index, Count, ElementSize) ScriptArray->Insert(Index, Count, ElementSize)
#define ScriptArray_Remove(ScriptArray, Count, Count, ElementSize)  ScriptArray->Remove(Count, Count, ElementSize)
#define ScriptArray_Empty(ScriptArray, Size, ElementSize)  ScriptArray->Empty(Size, ElementSize)
#endif

struct FCTArrayDynamicProperty : public FCDynamicProperty
{
    FArrayProperty* ArrayProperty;
    FCDynamicProperty *Inner;
    int ElementSize;
    FCTArrayDynamicProperty(FArrayProperty* InArrayProperty, FCDynamicProperty *ChildInner) :ArrayProperty(InArrayProperty), Inner(ChildInner), ElementSize(0)
    {
        ElementSize = ArrayProperty->Inner->GetSize();
    }
    ~FCTArrayDynamicProperty()
    {
        if (ArrayProperty)
        {
            // delete ArrayProperty; // 这个不能释放，UE会自动释放，不然就会Crash
        }
    }
};

struct FCTMapDynamicProperty : public FCDynamicProperty
{
    FMapProperty* MapProperty;
    FCTMapDynamicProperty(FMapProperty* InMapProperty) :MapProperty(InMapProperty)
    {
    }
    ~FCTMapDynamicProperty()
    {
        if (MapProperty)
        {
            //delete MapProperty; // 这个不能释放，UE会自动释放，不然就会Crash
        }
    }
};

struct FCTSetDynamicProperty : public FCDynamicProperty
{
    FSetProperty* SetProperty;
    FCTSetDynamicProperty(FSetProperty* InSetProperty) :SetProperty(InSetProperty)
    {
    }
    ~FCTSetDynamicProperty()
    {
        if (SetProperty)
        {
            //delete SetProperty; // 这个不能释放，UE会自动释放，不然就会Crash
        }
    }
};

FProperty  *CreateClassProperty(const char *InClassName);
FCDynamicProperty *GetCppDynamicProperty(const char *InClassName);
FCDynamicProperty* GetStructDynamicProperty(UStruct* Struct);
FCDynamicProperty* GetDynamicPropertyByUEProperty(FProperty* InProperty);

FArrayProperty* CreateTArrayProperty(const char *InPropertyType);

FCDynamicProperty *GetTArrayDynamicProperty(const char* InPropertyType);

FCDynamicProperty *GetTMapDynamicProperty(const char *KeyTypeName, const char *ValueTypeName );

FCDynamicProperty* GetTSetDynamicProperty(const char* KeyTypeName);

void ReleaseTempalteProperty();

void TArray_Clear(FScriptArray *ScriptArray, FProperty *Inner);

void TMap_Clear(FScriptMap* ScriptMap, FMapProperty* MapProperty);

void TSet_Clear(FScriptSet* ScriptMap, FSetProperty* SetProperty);

bool IsBaseTypeCopy(const FProperty *InPropery);