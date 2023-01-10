#include "FCDefaultParam.h"


void FCDefaultParamBool::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    if(DefaultParam2[0] == '1')
        DefaultValue = true;
    else if (DefaultParam2[0] == '0')
        DefaultValue = false;
    else if (strcmp(DefaultParam2, "true") == 0
        || strcmp(DefaultParam2, "TRUE") == 0)
        DefaultValue = true;
    else
        DefaultValue = false;
}

void FCDefaultParamBool::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((bool *)ValueAddr) = DefaultValue;
}

void FCDefaultParamByte::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    FByteProperty *Propety = (FByteProperty *)DynamicProperty->Property;
    if(Propety->Enum)
    {
        bEnum = true;
        DefaultValue = Propety->Enum->GetValueByName(FName(DefaultParam2));
    }
    else
    {
        bEnum = false;
        DefaultValue = (int8)atoi(DefaultParam2);
    }
}

void FCDefaultParamByte::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    if(bEnum)
    {
        switch(DynamicProperty->Type)
        {
            case FCPROPERTY_Int64Property:
            case FCPROPERTY_UInt64Property:
                *((int64*)ValueAddr) = DefaultValue;
                break;
            case FCPROPERTY_IntProperty:
            case FCPROPERTY_UInt32Property:
                *((int32*)ValueAddr) = (int32)DefaultValue;
                break;
            case FCPROPERTY_Int8Property:
            case FCPROPERTY_ByteProperty:
                *((int8*)ValueAddr) = (int8)DefaultValue;
                break;
            default:
                break;
        }
    }
    else
    {
        *((int8*)ValueAddr) = DefaultValue;
    }
}

void FCDefaultParamInt8::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = (int8)atoi(DefaultParam2);
}

void FCDefaultParamInt8::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((int8*)ValueAddr) = DefaultValue;
}

void FCDefaultParamInt16::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = (int16)atoi(DefaultParam2);
}

void FCDefaultParamInt16::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((int16*)ValueAddr) = DefaultValue;
}

void FCDefaultParamInt32::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = (int32)atoi(DefaultParam2);
}

void FCDefaultParamInt32::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((int32*)ValueAddr) = DefaultValue;
}

void FCDefaultParamInt64::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = (int64)atoi(DefaultParam2);
}

void FCDefaultParamInt64::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((int64*)ValueAddr) = DefaultValue;
}

void FCDefaultParamFloat::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = (float)atof(DefaultParam2);
}

void FCDefaultParamFloat::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((float*)ValueAddr) = DefaultValue;
}

void FCDefaultParamDouble::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = atof(DefaultParam2);
}

void FCDefaultParamDouble::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((double*)ValueAddr) = DefaultValue;
}

void FCDefaultParamFString::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = DefaltParam1;
}

void FCDefaultParamFString::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((FString*)ValueAddr) = DefaultValue;
}

void FCDefaultParamFName::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = FName(*DefaltParam1);
}

void FCDefaultParamFName::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((FName*)ValueAddr) = DefaultValue;
}

void FCDefaultParamText::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    DefaultValue = FText::FromString(DefaltParam1);
}

void FCDefaultParamText::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((FText*)ValueAddr) = DefaultValue;
}

void FCDefaultEnum::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    UEnum* Enum = FindObjectChecked<UEnum>(ANY_PACKAGE, UTF8_TO_TCHAR(DynamicProperty->ClassName));
    if(Enum)
        DefaultValue = Enum->GetValueByName(FName(DefaultParam2));
    else
        DefaultValue = 0;
}

void FCDefaultVector2D::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    FCStringBuffer128  Value;
    DefaultValue.X = Value.ReadFloat(DefaultParam2);
    DefaultValue.Y = Value.ReadFloat(DefaultParam2);
}

void FCDefaultVector2D::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((FVector2D*)ValueAddr) = DefaultValue;
}

void FCDefaultVector::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    FCStringBuffer128  Value;
    DefaultValue.X = Value.ReadFloat(DefaultParam2);
    DefaultValue.Y = Value.ReadFloat(DefaultParam2);
    DefaultValue.Z = Value.ReadFloat(DefaultParam2);
}

void FCDefaultVector::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((FVector*)ValueAddr) = DefaultValue;
}

void FCDefaultVector4::InitDefaultValue(FCDynamicProperty* DynamicProperty, const FString& DefaltParam1, const char* DefaultParam2)
{
    FCStringBuffer128  Value;
    DefaultValue.X = Value.ReadFloat(DefaultParam2);
    DefaultValue.Y = Value.ReadFloat(DefaultParam2);
    DefaultValue.Z = Value.ReadFloat(DefaultParam2);
    DefaultValue.W = Value.ReadFloat(DefaultParam2);
}

void FCDefaultVector4::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    *((FVector4*)ValueAddr) = DefaultValue;
}
void FCDefaultEnum::WriteDefaultValue(FCDynamicProperty* DynamicProperty, uint8* ValueAddr)
{
    switch (DynamicProperty->Type)
    {
    case FCPROPERTY_Int64Property:
    case FCPROPERTY_UInt64Property:
        *((int64*)ValueAddr) = DefaultValue;
        break;
    case FCPROPERTY_IntProperty:
    case FCPROPERTY_UInt32Property:
        *((int32*)ValueAddr) = (int32)DefaultValue;
        break;
    case FCPROPERTY_Int8Property:
    case FCPROPERTY_ByteProperty:
        *((int8*)ValueAddr) = (int8)DefaultValue;
        break;
    default:
        break;
    }
}

struct FCDefaultParamKey
{
    FCPropertyType  Type;
    const char *    InitValue;
    FCDefaultParamKey():Type(FCPropertyType::FCPROPERTY_Unkonw), InitValue(nullptr){}
    FCDefaultParamKey(FCPropertyType InType, const char* InInitValue) :Type(InType), InitValue(InInitValue) {}
};

struct FCDefaultParamKeyHash
{
    size_t operator()(const FCDefaultParamKey& Key) const
    {
        return (fc_hash_string_key::Hash(Key.InitValue) << 6) | Key.Type;
    }
};

struct FCDefaultParamKeyEqual
{
    bool operator()(const FCDefaultParamKey& key1, const FCDefaultParamKey& key2) const
    {     
        return key1.Type == key2.Type
            && key1.InitValue == key2.InitValue;
    }
};

typedef std::unordered_map<FCDefaultParamKey, FCDefaultParamBase*, FCDefaultParamKeyHash, FCDefaultParamKeyEqual> CDefaultParamMap;
CDefaultParamMap  GDefaultParamMap;

FCDefaultParamBase* GetDefaultValue(FCDynamicProperty* DynamicProperty, const FString& Value)
{
    const char *InitValue = TCHAR_TO_UTF8(*Value);
    InitValue = GetConstName(InitValue);
    FCDefaultParamKey Key(DynamicProperty->Type, InitValue);
    CDefaultParamMap::iterator  itFind = GDefaultParamMap.find(Key);
    if(itFind != GDefaultParamMap.end())
    {
        return itFind->second;
    }
    FCDefaultParamBase *DefaultParam = nullptr;
    switch(DynamicProperty->Type)
    {
        case FCPROPERTY_BoolProperty:
            DefaultParam = new FCDefaultParamBool();
            break;
        case FCPROPERTY_Int8Property:
            DefaultParam = new FCDefaultParamInt8();
            break;
        case FCPROPERTY_ByteProperty:
            DefaultParam = new FCDefaultParamByte();
            break;
        case FCPROPERTY_IntProperty:
        case FCPROPERTY_UInt32Property:
            DefaultParam = new FCDefaultParamInt32();
            break;
        case FCPROPERTY_Int64Property:
        case FCPROPERTY_UInt64Property:
            DefaultParam = new FCDefaultParamInt64();
            break;
        case FCPROPERTY_FloatProperty:
            DefaultParam = new FCDefaultParamFloat();
            break;
        case FCPROPERTY_DoubleProperty:
            DefaultParam = new FCDefaultParamDouble();
            break;
        case FCPROPERTY_StrProperty:
            DefaultParam = new FCDefaultParamFString();
            break;
        case FCPROPERTY_NameProperty:
            DefaultParam = new FCDefaultParamFName();
            break;
        case FCPROPERTY_TextProperty:
            DefaultParam = new FCDefaultParamText();
            break;
        case FCPROPERTY_Enum:
            DefaultParam = new FCDefaultEnum();
            break;
        case FCPROPERTY_Vector2:
            DefaultParam = new FCDefaultVector2D();
            break;
        case FCPROPERTY_Vector3:
            DefaultParam = new FCDefaultVector();
            break;
        case FCPROPERTY_Vector4:
            DefaultParam = new FCDefaultVector4();
            break;
        default:
            break;
    }
    if(DefaultParam)
    {
        DefaultParam->InitDefaultValue(DynamicProperty, Value, InitValue);
    }
    GDefaultParamMap[Key] = DefaultParam;
    return DefaultParam;
}

void ClearAllDefaultValue()
{
    ReleasePtrMap(GDefaultParamMap);
}

