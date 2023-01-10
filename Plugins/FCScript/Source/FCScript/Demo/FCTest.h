#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "FCTest.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedEvent);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnHttpResponseMessage, FString, MessageContent, bool, bWasSuccessful);

USTRUCT(BlueprintType)
struct FTestBoneAdjustItemInfo
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString   SlotName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32     ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName     BoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector   Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector   Offset;
};

USTRUCT(BlueprintType)
struct FTestAvatarSystemInitParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> HideBoneWhiteList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MaleFaceConfigPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTestBoneAdjustItemInfo>  BoneAdjustItemsTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTestBoneAdjustItemInfo   BoneAdjustItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector   Offset;
};

UENUM(BlueprintType)
enum EFCTestEnum
{
    One,
    Two,
    Three,
    Wan = 10000,  // 测试发现，UE不支持255以上的默认参数
};

UCLASS()
class UFCTest : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHP() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetIDList(const TArray<int32> &IDs);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool GetIDList(TArray<int32> &IDs);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SetIDMap(const TMap<int32, int32> &IDs);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void GetIDS(TArray<int32>& OutIDS, TMap<int32, int32> &OutMap);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SetIDSet(const TSet<int32> &InIDSet);

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void GetIDSet(TSet<int32>& OutIDSet);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetNameList(const TArray<FString> &Names);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "NotifyObject"))
	void NotifyObject(int nType, float x, float y, float z);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "NotifyEvent"))
    int NotifyIDList(TArray<int32>& OutIDList);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "NotifyEvent"))
    FString NotifyAvatarParam(FTestAvatarSystemInitParams& AvatarParam);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "NotifyEvent"))
    int NotifyIDSet(TSet<int32>& OutIDSet);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "NotifyEvent"))
    int NotifyIDMap(TMap<int32, int32>& OutIDMap);

    UFUNCTION(BlueprintCallable, Category = "TestCall")
    void TestCall_NotifyIDList();

    UFUNCTION(BlueprintCallable, Category = "TestCall")
    void TestCall_NotifyAvatarParam();

    UFUNCTION(BlueprintCallable, Category = "TestCall")
    void TestCall_NotifyIDSet();

    UFUNCTION(BlueprintCallable, Category = "TestCall")
    void TestCall_NotifyIDMap();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "NotifyAll"))
	static int NotifyAll(int nType, const FVector &Pos);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "TestCall_DefaultParam"))
    void TestCall_DefaultParam(int InID = 10, const FString InName = TEXT("ABC"), EFCTestEnum TestType = EFCTestEnum::Wan, FVector InPos = FVector(3, 5, 6));

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "HttpNotify"))
	void HttpNotify(const FString &MessageContent, bool bWasSuccessful);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CallClicked"))
	void CallClicked();

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetActor"))
    void SetActor(AActor *Actor);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetAvatarParam"))
    void SetAvatarParam(const FTestAvatarSystemInitParams &Param);	
	
	//UPROPERTY()
	//TSharedPtr<UFCTest> SharedPtr;  // 这个是不支持的

	UPROPERTY()
	TWeakObjectPtr<UFCTest>   WeakPtr;

	UPROPERTY()
	TLazyObjectPtr<UFCTest>  LazyPtr;
	
	UPROPERTY()
	TSoftObjectPtr<UObject>  ResPtr;

    UPROPERTY(BlueprintReadWrite)
    TSoftClassPtr<UClass> TSoftClassPtrVar;

    //UPROPERTY(BlueprintReadWrite)
    //TScriptInterface<IFCTestInterface> TScriptInterfaceVar;
    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<UObject> TSubclassOfVar;

    UPROPERTY(BlueprintReadWrite)
    TEnumAsByte<enum EFCTestEnum> EnumAsByteVar;

	UPROPERTY()
	class UFCTest* NextPtr;
	UPROPERTY()
	int ID;
    UPROPERTY()
    FString NameValue;
	UPROPERTY()
	float HP;
	UPROPERTY()
	int aID[3];
	UPROPERTY()
	FVector Pos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> NameList;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> IDList;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<int32, int32> IDMap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<int32> IDSet;

	UPROPERTY(BlueprintAssignable, Category = "Button|Event")
	FOnButtonClickedEvent OnClicked;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FOnHttpResponseMessage OnResponseMessage;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FTestAvatarSystemInitParams   AvatarParams;
};
