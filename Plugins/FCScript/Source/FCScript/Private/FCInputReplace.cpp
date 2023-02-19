#include "FCInputReplace.h"
#include "FCObjectManager.h"
#include "FCDelegateObject.h"
#include "FCDynamicDelegateManager.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Components/InputComponent.h"


#if ENGINE_MAJOR_VERSION <= 4 && ENGINE_MINOR_VERSION < 20
#define GET_INPUT_ACTION_NAME(IAB) IAB.ActionName
#define IS_INPUT_ACTION_PAIRED(IAB) IAB.bPaired
#else
#define GET_INPUT_ACTION_NAME(IAB) IAB.GetActionName()
#define IS_INPUT_ACTION_PAIRED(IAB) IAB.IsPaired()
#endif

static const TCHAR* SReadableInputEvent[] = { TEXT("Pressed"), TEXT("Released"), TEXT("Repeat"), TEXT("DoubleClick"), TEXT("Axis"), TEXT("Max") };

FCInputReplace  &FCInputReplace::GetIns()
{
    static FCInputReplace  sIns;
    return sIns;
}

void FCInputReplace::Clear()
{

}

bool FCInputReplace::ReplaceInputs(AActor* Actor, class UInputComponent* InputComponent)
{
    if (!Actor || !InputComponent)
        return false;
    // 如果没有绑脚本，就返回吧
    if(!FFCObjectdManager::GetSingleIns()->IsBindScript(Actor))
    {
        return false;
    }
    const UClass* Class = Actor->GetClass();

    ReplaceActionInputs(Actor, InputComponent);       // replace action inputs
    ReplaceKeyInputs(Actor, InputComponent);          // replace key inputs
    ReplaceAxisInputs(Actor, InputComponent);         // replace axis inputs
    ReplaceTouchInputs(Actor, InputComponent);        // replace touch inputs
    ReplaceAxisKeyInputs(Actor, InputComponent);      // replace AxisKey inputs
    ReplaceVectorAxisInputs(Actor, InputComponent);   // replace VectorAxis inputs
    ReplaceGestureInputs(Actor, InputComponent);      // replace gesture inputs

    return true;
}

UFunction* FCInputReplace::FindFuncByClass(const char* InFuncName)
{
    const UClass* Class = UFCDelegateObject::StaticClass();
    FName  FuncName(InFuncName);
    UFunction* Func = Class->FindFunctionByName(FuncName);
    return Func;
}

void FCInputReplace::ReplaceActionInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction *InputActionFunc = FindFuncByClass("InputAction");
    if (!InputActionFunc)
        return;

    TSet<FName> ActionNames;
    int32 NumActionBindings = InputComponent->GetNumActionBindings();
    for (int32 i = 0; i < NumActionBindings; ++i)
    {
        FInputActionBinding& IAB = InputComponent->GetActionBinding(i);
        FName Name = GET_INPUT_ACTION_NAME(IAB);
        FString ActionName = Name.ToString();
        ActionNames.Add(Name);

        FName FuncName = FName(*FString::Printf(TEXT("%s_%s"), *ActionName, SReadableInputEvent[IAB.KeyEvent]));
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                IAB.ActionDelegate.BindDelegate(FuncHost, FuncName);
            }
        }

        if (!IS_INPUT_ACTION_PAIRED(IAB))
        {
            EInputEvent IE = IAB.KeyEvent == IE_Pressed ? IE_Released : IE_Pressed;
            FuncName = FName(*FString::Printf(TEXT("%s_%s"), *ActionName, SReadableInputEvent[IE]));
            if (IsHaveLuaFunction(Actor, FuncName))
            {
                UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
                if(FuncHost)
                {
                    FInputActionBinding AB(Name, IE);
                    AB.ActionDelegate.BindDelegate(FuncHost, FuncName);
                    InputComponent->AddActionBinding(AB);
                }
            }
        }
    }

    TSet<FName> DefaultActionNames;
    UInputSettings* DefaultIS = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
    TArray<FName> TempDefaultActionNames;
    DefaultIS->GetActionNames(TempDefaultActionNames);
    for (auto ActionName : TempDefaultActionNames)
    {
        DefaultActionNames.Add(ActionName);
    }

    EInputEvent IEs[] = { IE_Pressed, IE_Released };
    TSet<FName> DiffActionNames = DefaultActionNames.Difference(ActionNames);
    for (TSet<FName>::TConstIterator It(DiffActionNames); It; ++It)
    {
        FName ActionName = *It;
        for (int32 i = 0; i < 2; ++i)
        {
            FName FuncName = FName(*FString::Printf(TEXT("%s_%s"), *ActionName.ToString(), SReadableInputEvent[IEs[i]]));
            if (IsHaveLuaFunction(Actor, FuncName))
            {
                UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
                if(FuncHost)
                {
                    FInputActionBinding AB(ActionName, IEs[i]);
                    AB.ActionDelegate.BindDelegate(FuncHost, FuncName);
                    InputComponent->AddActionBinding(AB);
                }
            }
        }
    }
}

void FCInputReplace::ReplaceKeyInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("InputAction");
    if (!InputActionFunc)
        return;

    TArray<FKey> Keys;
    TArray<bool> PairedKeys;
    TArray<EInputEvent> InputEvents;
    for (FInputKeyBinding& IKB : InputComponent->KeyBindings)
    {
        int32 Index = Keys.Find(IKB.Chord.Key);
        if (Index == INDEX_NONE)
        {
            Keys.Add(IKB.Chord.Key);
            PairedKeys.Add(false);
            InputEvents.Add(IKB.KeyEvent);
        }
        else
        {
            PairedKeys[Index] = true;
        }

        FName FuncName = FName(*FString::Printf(TEXT("%s_%s"), *IKB.Chord.Key.ToString(), SReadableInputEvent[IKB.KeyEvent]));
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                IKB.KeyDelegate.BindDelegate(FuncHost, FuncName);
            }
        }
    }

    for (int32 i = 0; i < Keys.Num(); ++i)
    {
        if (!PairedKeys[i])
        {
            EInputEvent IE = InputEvents[i] == IE_Pressed ? IE_Released : IE_Pressed;
            FName FuncName = FName(*FString::Printf(TEXT("%s_%s"), *Keys[i].ToString(), SReadableInputEvent[IE]));
            if (IsHaveLuaFunction(Actor, FuncName))
            {
                UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
                if(FuncHost)
                {
                    FInputKeyBinding IKB(FInputChord(Keys[i]), IE);
                    IKB.KeyDelegate.BindDelegate(FuncHost, FuncName);
                    InputComponent->KeyBindings.Add(IKB);
                }
            }
        }
    }

    TArray<FKey> AllKeys;
    EKeys::GetAllKeys(AllKeys);     // get all key inputs
    EInputEvent IEs[] = { IE_Pressed, IE_Released };
    for (const FKey& Key : AllKeys)
    {
        if (Keys.Find(Key) != INDEX_NONE)
        {
            continue;
        }
        for (int32 i = 0; i < 2; ++i)
        {
            FName FuncName = FName(*FString::Printf(TEXT("%s_%s"), *Key.ToString(), SReadableInputEvent[IEs[i]]));
            if (IsHaveLuaFunction(Actor, FuncName))
            {
                UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
                if(FuncHost)
                {
                    FInputKeyBinding IKB(FInputChord(Key), IEs[i]);
                    IKB.KeyDelegate.BindDelegate(FuncHost, FuncName);
                    InputComponent->KeyBindings.Add(IKB);
                }
            }
        }
    }

}

void FCInputReplace::ReplaceAxisInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("InputAxis");
    if (!InputActionFunc)
        return;

    bool bNoneCallByZeroParam = true;
    TSet<FName> AxisNames;
    for (FInputAxisBinding& IAB : InputComponent->AxisBindings)
    {
        AxisNames.Add(IAB.AxisName);
        FName FuncName = IAB.AxisName;
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName, bNoneCallByZeroParam);
            if(FuncHost)
            {
                IAB.AxisDelegate.BindDelegate(Actor, FuncName);
            }
        }
    }

    TSet<FName> DefaultAxisNames;

    UInputSettings* DefaultIS = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
    TArray<FName> TempAxisNames;
    DefaultIS->GetAxisNames(TempAxisNames);
    for (auto AxisName : TempAxisNames)
    {
        DefaultAxisNames.Add(AxisName);
    }

    TSet<FName> DiffAxisNames = DefaultAxisNames.Difference(AxisNames);
    for (TSet<FName>::TConstIterator It(DiffAxisNames); It; ++It)
    {
        FName FuncName = *It;
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName, bNoneCallByZeroParam);
            if(FuncHost)
            {
                FInputAxisBinding& IAB = InputComponent->BindAxis(*It);
                IAB.AxisDelegate.BindDelegate(FuncHost, *It);
            }
        }
    }
}

void FCInputReplace::ReplaceTouchInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("InputTouch");
    if (!InputActionFunc)
        return;

    TArray<EInputEvent> InputEvents = { IE_Pressed, IE_Released, IE_Repeat };        // IE_DoubleClick?
    for (FInputTouchBinding& ITB : InputComponent->TouchBindings)
    {
        InputEvents.Remove(ITB.KeyEvent);
        FName FuncName = FName(*FString::Printf(TEXT("Touch_%s"), SReadableInputEvent[ITB.KeyEvent]));
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                ITB.TouchDelegate.BindDelegate(Actor, FuncName);
            }
        }
    }

    for (EInputEvent IE : InputEvents)
    {
        FName FuncName = FName(*FString::Printf(TEXT("Touch_%s"), SReadableInputEvent[IE]));
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                FInputTouchBinding ITB(IE);
                ITB.TouchDelegate.BindDelegate(FuncHost, FuncName);
                InputComponent->TouchBindings.Add(ITB);
            }
        }
    }
}

void FCInputReplace::ReplaceAxisKeyInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("InputAxis");
    if (!InputActionFunc)
        return;
    for (FInputAxisKeyBinding& IAKB : InputComponent->AxisKeyBindings)
    {
        FName FuncName = IAKB.AxisKey.GetFName();
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                IAKB.AxisDelegate.BindDelegate(FuncHost, FuncName);
            }
        }
    }
}

void FCInputReplace::ReplaceVectorAxisInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("InputVectorAxis");
    if (!InputActionFunc)
        return;
    for (FInputVectorAxisBinding& IVAB : InputComponent->VectorAxisBindings)
    {
        FName FuncName = IVAB.AxisKey.GetFName();
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                IVAB.AxisDelegate.BindDelegate(Actor, FuncName);
            }
        }
    }
}

void FCInputReplace::ReplaceGestureInputs(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("InputGesture");
    if (!InputActionFunc)
        return;
    for (FInputGestureBinding& IGB : InputComponent->GestureBindings)
    {
        FName FuncName = IGB.GestureKey.GetFName();
        if (IsHaveLuaFunction(Actor, FuncName))
        {
            UObject* FuncHost = OverridenActionFunction(Actor, InputActionFunc, FuncName);
            if(FuncHost)
            {
                IGB.GestureDelegate.BindDelegate(Actor, FuncName);
            }
        }
    }
}

void FCInputReplace::ReplaceAnimInstance(AActor* Actor, UInputComponent* InputComponent)
{
    UFunction* InputActionFunc = FindFuncByClass("TriggerAnimNotify");
    if (!InputActionFunc)
        return;
    // 这个感觉没有必要，在蓝图添加一个接口，然后Overriden就可以了
    //UClass *Class = Actor->GetClass();
    //if (Class->IsChildOf<UAnimInstance>())
    //{
    //    for (const auto& LuaFuncName : BindInfo.LuaFunctions)
    //    {
    //        if (!BindInfo.UEFunctions.Find(LuaFuncName) && LuaFuncName.ToString().StartsWith(TEXT("AnimNotify_")))
    //            ULuaFunction::Override(AnimNotifyFunc, Class, LuaFuncName);
    //    }
    //}
}

void FCInputReplace::OverridenTriggerAnimNotify(UObject* BindObject, const char* LuaFuncName, int64 InScriptIns)
{
    UFunction* InputActionFunc = FindFuncByClass("TriggerAnimNotify");
    if (!InputActionFunc)
        return;
    lua_State* L = GetScriptContext()->m_LuaState;
    FCDynamicDelegateManager::GetIns().OverridenLuaFunction(BindObject, BindObject, L, InScriptIns, InputActionFunc, LuaFuncName, false);
}

bool FCInputReplace::IsHaveLuaFunction(AActor* Actor, const FName &FuncName)
{
    const FBindObjectInfo* BindInfo = FFCObjectdManager::GetSingleIns()->FindBindObjectInfo(Actor);
    if(!BindInfo)
    {
        return false;
    }
    if(BindInfo->m_ScriptIns <= 0)
        return false;
    lua_State*L = GetScriptContext()->m_LuaState;
    FString value = FuncName.ToString();
    lua_rawgeti(L, LUA_REGISTRYINDEX, BindInfo->m_ScriptIns);
    lua_pushstring(L, TCHAR_TO_UTF8(*value));
    lua_rawget(L, -2);

    int Type = lua_type(L, -1);
    lua_pop(L, 2);
    return LUA_TFUNCTION == Type;
}

UObject* FCInputReplace::OverridenActionFunction(AActor* Actor, UFunction* ActionFunc, const FName &FuncName, bool bNoneCallByZeroParam)
{
// 为了不污染Actor, 全部改成到UFCDelegateObject对象，再由这个转调到这里来
    lua_State* L = GetScriptContext()->m_LuaState;
    const FBindObjectInfo* BindInfo = FFCObjectdManager::GetSingleIns()->FindBindObjectInfo(Actor);
    UObject* Outer = GetScriptContext()->m_DelegateObject;
    return FCDynamicDelegateManager::GetIns().OverridenLuaFunction(Actor, Outer, L, BindInfo->m_ScriptIns, ActionFunc, FuncName, bNoneCallByZeroParam);
}