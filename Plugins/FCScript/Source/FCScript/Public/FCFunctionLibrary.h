// Copyright (c) Tencent Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FCFunctionLibrary.generated.h"

UCLASS()
class FCSCRIPT_API UFCFunctionLibrary :public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Lua", CustomThunk, meta = (BlueprintInternalUseOnly = "true", Variadic))
    static bool CallLuaTableFunction(const FString& LuaFileName, const FString& Function, bool SelfCall);
    DECLARE_FUNCTION(execCallLuaTableFunction);

    UFUNCTION(BlueprintCallable, Category = "Lua", CustomThunk, meta = (BlueprintInternalUseOnly = "true", Variadic))
    static bool CallLuaFunction(const FString& Function);
    DECLARE_FUNCTION(execCallLuaFunction);
};
