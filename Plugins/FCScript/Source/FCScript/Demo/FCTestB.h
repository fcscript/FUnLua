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

    UFUNCTION(BlueprintGetter)
    int GetUserID() const;

    UFUNCTION(BlueprintSetter)
    void SetUserID(int NewUserID) { UserID = NewUserID; }

protected:
    UPROPERTY(EditAnywhere, BlueprintSetter = SetUserID, BlueprintGetter = GetUserID, Category = "Attributes")
    int UserID;
};