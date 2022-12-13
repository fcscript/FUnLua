#pragma once

#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject.h"

namespace FCScript
{
    template <class _Ty>
    FORCEINLINE const char* ExtractTypeName(const _Ty&)
    {
        return "";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const bool&)
    {
        return "bool";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const int8&)
    {
        return "int8";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const uint8&)
    {
        return "uint8";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const int16&)
    {
        return "int16";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const uint16&)
    {
        return "int16";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const int&)
    {
        return "int";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const uint32&)
    {
        return "uint";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const int64&)
    {
        return "int64";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const uint64&)
    {
        return "uint64";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const float&)
    {
        return "float";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const double&)
    {
        return "double";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FString&)
    {
        return "FString";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FName&)
    {
        return "FName";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FIntPoint&)
    {
        return "FIntPoint";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FIntVector&)
    {
        return "FIntVector";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FRotator&)
    {
        return "FRotator";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FQuat&)
    {
        return "FQuat";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FTransform&)
    {
        return "FTransform";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FLinearColor&)
    {
        return "FLinearColor";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FColor&)
    {
        return "FColor";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FPlane&)
    {
        return "FPlane";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FVector&)
    {
        return "FVector";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FVector2D&)
    {
        return "FVector2D";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FVector4&)
    {
        return "FVector4";
    }

    template <> FORCEINLINE const char* ExtractTypeName(const FGuid&)
    {
        return "FGuid";
    }
}