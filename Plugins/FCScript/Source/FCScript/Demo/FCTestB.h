#pragma once

#include "FCTest.h"
#include "FCTestB.generated.h"


UCLASS()
class UFCTestB : public UFCTest
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class UObject* BasePtr;

public:
    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SetBasePtr(UFCTestB *Obj, UObject *Ptr);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SetPtr(UObject* Ptr);
};