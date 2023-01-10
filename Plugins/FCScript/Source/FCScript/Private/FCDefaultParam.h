#pragma once

#include "FCDynamicClassDesc.h"

struct FCDefaultParamBool : public FCDefaultParamBase
{
    bool   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamByte : public FCDefaultParamBase
{
    int64   DefaultValue;
    bool    bEnum;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamInt8 : public FCDefaultParamBase
{
    int8   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamInt16 : public FCDefaultParamBase
{
    int16   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamInt32 : public FCDefaultParamBase
{
    int32   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamInt64 : public FCDefaultParamBase
{
    int64   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamFloat : public FCDefaultParamBase
{
    float   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamDouble : public FCDefaultParamBase
{
    double   DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamFString : public FCDefaultParamBase
{
    FString  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamFName : public FCDefaultParamBase
{
    FName  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultParamText : public FCDefaultParamBase
{
    FText  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultEnum : public FCDefaultParamBase
{
    int64  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultVector2D : public FCDefaultParamBase
{
    FVector2D  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultVector : public FCDefaultParamBase
{
    FVector  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

struct FCDefaultVector4 : public FCDefaultParamBase
{
    FVector4  DefaultValue;
    virtual void InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2);
    virtual void WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr);
};

FCDefaultParamBase*  GetDefaultValue(FCDynamicProperty* DynamicProperty, const FString &Value);
void ClearAllDefaultValue();