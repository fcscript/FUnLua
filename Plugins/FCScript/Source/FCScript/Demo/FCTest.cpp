#include "FCTest.h"

float UFCTest::GetHP() const
{
	return HP;
}

void UFCTest::SetIDList(const TArray<int32> &IDs)
{
	IDList = IDs;
}

bool UFCTest::GetIDList(TArray<int32> &IDs)
{
	IDs = IDList;
	return true;
}

void UFCTest::SetIDMap(const TMap<int32, int32>& IDs)
{
    IDMap = IDs;
}

void UFCTest::GetIDS(TArray<int32>& OutIDS, TMap<int32, int32>& OutMap)
{
    OutIDS = IDList;
    OutMap = IDMap;
}

void UFCTest::SetIDSet(const TSet<int32>& InIDSet)
{
    IDSet = InIDSet;
}

void UFCTest::GetIDSet(TSet<int32>& OutIDSet)
{
    OutIDSet = IDSet;
}

void UFCTest::SetNameList(const TArray<FString> &Names)
{
	NameList = Names;
}

void UFCTest::TestCall_NotifyIDList()
{
    TArray<int32>  OutIDList;
    OutIDList.Add(20);
    OutIDList.Add(21);
    OutIDList.Add(22);
    int Ret = NotifyIDList(OutIDList);
    IDList = OutIDList;
}

void UFCTest::TestCall_NotifyAvatarParam()
{
    FTestAvatarSystemInitParams  Params;
    Params.MaleFaceConfigPath = TEXT("TestPath");
    FString Ret = NotifyAvatarParam(Params);
    AvatarParams = Params;
}

void UFCTest::TestCall_NotifyIDSet()
{
    TSet<int32>  OutIDSet;
    OutIDSet.Add(30);
    OutIDSet.Add(31);
    int Ret = NotifyIDSet(OutIDSet);
    IDSet = OutIDSet;
}

void UFCTest::TestCall_NotifyIDMap()
{
    TMap<int32, int32>  OutIDMap;
    OutIDMap.Add(2) = 20;
    OutIDMap.Add(3) = 30;
    int Ret = NotifyIDMap(OutIDMap);
    IDMap = OutIDMap;
}

int UFCTest::NotifyAll(int nType, const FVector &Pos)
{
	return 100 + nType;
}

void UFCTest::TestCall_DefaultParam(int InID, const FString InName, EFCTestEnum TestType, FVector InPos)
{
    ID = InID;
    NameValue = InName;
    EnumAsByteVar = TestType;
}

void UFCTest::HttpNotify(const FString &MessageContent, bool bWasSuccessful)
{
	OnResponseMessage.ExecuteIfBound(MessageContent, bWasSuccessful);
}

void UFCTest::CallClicked()
{
	OnClicked.Broadcast();
}

void UFCTest::SetActor(AActor* Actor)
{
    if(Actor)
    {
#if WITH_EDITORONLY_DATA
        Actor->bHiddenEdLevel = true;
        Actor->Tags.Add(FName("Test"));
#endif
    }    
}

void UFCTest::SetAvatarParam(const FTestAvatarSystemInitParams& Param)
{
    AvatarParams = Param;
}

void UFCTest::SetSoftPtr(TSoftObjectPtr<UObject> Arg1, TSoftClassPtr<UClass> Arg2)
{
    ResPtr = Arg1;
    TSoftClassPtrVar = Arg2;
}