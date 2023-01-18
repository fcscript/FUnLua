
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
};
