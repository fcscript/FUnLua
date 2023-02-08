
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "FCDelegateObject.generated.h"


UCLASS()
class UFCDelegateObject : public UObject
{
    GENERATED_BODY()
public:
    virtual void BeginDestroy() override;
public:
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "DelegateHelper"))
    void OnDelegateExcute();
public:
    UFUNCTION(BlueprintImplementableEvent)
    void EnhancedInputActionDigital(bool ActionValue, float ElapsedSeconds, float TriggeredSeconds);

    UFUNCTION(BlueprintImplementableEvent)
    void EnhancedInputActionAxis1D(float ActionValue, float ElapsedSeconds, float TriggeredSeconds);

    UFUNCTION(BlueprintImplementableEvent)
    void EnhancedInputActionAxis2D(const FVector2D& ActionValue, float ElapsedSeconds, float TriggeredSeconds);

    UFUNCTION(BlueprintImplementableEvent)
    void EnhancedInputActionAxis3D(const FVector& ActionValue, float ElapsedSeconds, float TriggeredSeconds);

    UFUNCTION(BlueprintImplementableEvent)
    void InputAction(FKey Key);

    UFUNCTION(BlueprintImplementableEvent)
    void InputAxis(float AxisValue);

    UFUNCTION(BlueprintImplementableEvent)
    void InputTouch(ETouchIndex::Type FingerIndex, const FVector& Location);

    UFUNCTION(BlueprintImplementableEvent)
    void InputVectorAxis(const FVector& AxisValue);

    UFUNCTION(BlueprintImplementableEvent)
    void InputGesture(float Value);

    UFUNCTION(BlueprintImplementableEvent)
    void TriggerAnimNotify();
};
