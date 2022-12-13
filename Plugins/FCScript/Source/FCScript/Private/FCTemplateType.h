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