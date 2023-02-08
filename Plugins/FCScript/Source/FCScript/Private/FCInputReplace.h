#pragma once

#include "InputCoreTypes.h"
#include "Engine/DynamicBlueprintBinding.h"

class FCInputReplace
{
public:
    static FCInputReplace &GetIns();
public:
    void Clear();
    bool ReplaceInputs(AActor* Actor, class UInputComponent* InputComponent);
private:
    UFunction *FindFuncByClass(const UClass *Class, const char *InFuncName);
    void ReplaceActionInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceKeyInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceAxisInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceTouchInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceAxisKeyInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceVectorAxisInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceGestureInputs(AActor* Actor, UInputComponent* InputComponent);
    void ReplaceAnimInstance(AActor* Actor, UInputComponent* InputComponent);
    bool IsHaveLuaFunction(AActor* Actor, const FName &FuncName);
    UObject* OverridenActionFunction(AActor* Actor, UFunction *ActionFunc, const FName &FuncName, bool bNoneCallByZeroParam = false);
};