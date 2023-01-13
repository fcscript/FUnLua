local  TestCrash = {}
TestCrash.__index = TestCrash


function GetGameInstanceByContextObject(worldContextObject)
    local gameInstance = UE4.UGameplayStatics.GetGameInstance(worldContextObject)
    return gameInstance
end

local function GetGameTimeSecond(worldContextObject)
    local gameInstance = UE4.UGameplayStatics.GetGameInstance(worldContextObject)
    local  world = gameInstance:GetWorld()
    return UE4.UKismetSystemLibrary.GetGameTimeInSeconds(world)
end

local function CreateDefaultTransform()
    local pos = UE4.FVector(0, 0, 0)
    local rotator = UE4.FRotator(0, 0, 0)
    local scale = UE4.FVector(1, 1, 1)
    local transform = UE4.UKismetMathLibrary.MakeTransform(pos, rotator, scale)        
    return transform
end

-- 创建一个测试角色
local function CreateTestActor(assetName, moduleName, worldContextObject)  
    UEPrint("[TestCrash]CreateTestActor----1")
    local gameInstance = _G.GetGameInstanceByContextObject(worldContextObject)
    local world = gameInstance:GetWorld()
    if not assetName then
        assetName = "/Game/TestActorBP.TestActorBP_C"
    end

    UEPrint("[TestCrash]CreateTestActor----2, world=", world)
    local playerBPClass = UE4.UClass.Load(assetName)
    local transform = CreateDefaultTransform()
    UEPrint("[TestCrash]CreateTestActor----3, playerBPClass=", playerBPClass)
    local localActor = world:SpawnActor(playerBPClass, transform, UE4.ESpawnActorCollisionHandlingMethod.AlwaysSpawn, nil, nil, moduleName)
    return localActor
end

-- 崩溃示例:两个不同类型的结构体做加法  结果:
function TestCrash:Crash1(worldContextObject)
    for i = 1, 1000 do
        local c1 = UE4.FColor(1, 1, 1, 1)
        local c2 = UE4.FVector4(2, 2, 2, 2)
        local c = c1 + c2
        local c3 = c2 + c1
        UE4.FVector4.Set(c1, 4, 4, 4, 4)
    end
    UEPrint("[TestCrash]run Crash1")
end

function TestCrash:DelayDeleteActor() 
    self.TestActor:K2_DestroyActor()
    UE4.UKismetSystemLibrary.CollectGarbage()  -- 立即GC一下
end

function TestCrash:SetActorPos()    
    local headPos = UE4.FVector(1, 1, 1)
    local rotator = UE4.FRotator(0, 0, 0)
    local sweepHitResult = UE4.FHitResult()
    self.TestActor:K2_SetActorLocationAndRotation(headPos, rotator, false,sweepHitResult, false)
end

-- 延迟删除
function TestCrash:DeleteTestActor(delayTime)    
    local  spTimer = _G.SPAvatarScene.SPTimer
    spTimer:Schedule(handle(self, TestCrash.DelayDeleteActor), delayTime or 0.01)
end

function TestCrash:DelayOpenMap(worldContextObject)
    UE4.UGameplayStatics.OpenLevel(worldContextObject, "HotPatch", true, "")
end

function TestCrash:SwitchHotPath(delayTime, worldContextObject)
    -- local  spTimer = _G.SPAvatarScene.SPTimer
    -- spTimer:Schedule(handle2(self, TestCrash.DelayOpenMap, worldContextObject), delayTime or 0.01)
end

-- 崩溃示例 : 测试Unlua持有UObject对象，但UObject对象释放的情况，后续对对成员赋值的情况
-- 说明：由于有GC延时问题，这个用例
-- Class_NewIndex
-- Class_CallUFunction
-- 这个测试结果是没有问题，并不会引用非法的写入
-- 如果去掉GetUObject 中的UE_BUILD_DEBUG检测，则在切场景的 OnPostGarbageCollect 函数中会崩溃，但这个是在Debug版本
function TestCrash:Crash2(worldContextObject)     
    UEPrint("[TestCrash]run Crash2-----begin")
    if self.TestActor then        
        self.TestActor.FolderPath = "11/22/33/44/55/66/77/88"
        local oldPos = self.TestActor:K2_GetActorLocation()
        self:SetActorPos()
        self.TestActor.FolderPath = "11/22/33/44/55/66/77/88/99/10"
        self.TestActor:Modify()
    else
        UEPrint("[Crash]Crash2 ----2")
        self.TestActor = CreateTestActor(nil, "UnLua.UI.ActorCallback", worldContextObject)
        self:DeleteTestActor(1)
    end
    UEPrint("[TestCrash]run Crash2")
end

-- 崩溃示例：测试对象释放，将该UObject对象，当作参数，传给C++使用
-- Class_NewIndex
-- 这个测试结果是没有问题，并不会引用非法的写入
function TestCrash:Crash3(worldContextObject)
    if self.TestPlayer then
        local results = UE4.TArray(UE4.AActor)
        UE4.UGameplayStatics.GetAllActorsWithTag(self.TestPlayer, "aaa" ,results)
        self.TestPlayer.FolderPath = "11/22/33/44/55/66/77/88/99/10"
        UE4.UGameplayStatics.GetAllActorsWithTag(self.BPChatItemClass, "aa", results)
        local rotator = UE4.FRotator(0, 0, 0)
        self.TestPlayer:SetTargetRotation(rotator, false)
        self.TestPlayer:SetCanActorLookAt(true)
    else
        self.TestPlayer = CreateTestActor("/Game/TestActorBP.TestActorBP_C", nil, worldContextObject)
        self:SwitchHotPath(worldContextObject)
        self.BPChatItemClass = UE4.UClass.Load("/Game/DataAsset.DataAsset_C")
        UEPrint("[TestCrash]run Crash3, TestPlayer=", self.TestPlayer, ",self.BPChatItemClass=", self.BPChatItemClass)
    end
    UEPrint("[TestCrash]run Crash3")
end

-- 崩溃示例：测试不同struct的参数使用, 还有参数错误的情况
-- 说明：这个会崩溃
function TestCrash:Crash4(worldContextObject)
    local t1 = UE4.FTransform(UE4.FVector(0.0, 0.0, 0.0), UE4.FVector(0.0, 0.0, 0.0), 1)
    local t2 = UE4.FTransform(UE4.FColor(0, 1, 2), UE4.FVector(0.0, 0.0, 0.0), 1)
    t1.Blend(t2)
    UEPrint("[TestCrash]run Crash4")
end

-- 崩溃示例：错误的参数, 将一个错误UObject类型参数传递，是会崩溃的
-- Class_CallUFunction
-- 结果：异常崩溃
function TestCrash:Crash5(worldContextObject)   
    local tempActor = CreateTestActor(nil, nil, worldContextObject)
    local sweepHitResult = UE4.FHitResult()
    local trans = CreateDefaultTransform()
    local c = UE4.FColor(0, 1, 2)
    local classObj = UE4.UClass.Load("/Game/DataAsset.DataAsset_C")
    tempActor.SetActorEnableCollision(classObj, false)  -- 测试发现，这个UnLua里面没有安全检查，会强转，但可能会Crash, 测试没有出现Crash
    tempActor.SetActorEnableCollision(classObj, true)
    classObj.NetDriverName = "bbbbb"  -- 非法访问，会Crash
    UEPrint("[TestCrash]run Crash5, classObj.NetDriverName=", classObj.NetDriverName)
end

-- TArray崩溃
-- 这种强转的，都会崩溃，像TMap, 等等
-- 结果：异常崩溃
function TestCrash:Crash6(worldContextObject)
    local results = UE4.TArray(UE4.AActor)
    local c = UE4.FColor(0, 1, 2)
    UE4.TArray.Add(c, 1)
    UEPrint("[TestCrash]run Crash6")
end

-- UObject反射对象的属性引用(最危险行为)
-- 在LUA端引用UObject属性，然后在对象释放后，调用其成员
-- 说明：如果引用的还是UObject对象，由于UObject释放时，会自动从LUA中注销，所以能检测到, 但非UObject则不能
-- 结果：异常崩溃
function TestCrash:Crash7(worldContextObject)
    UEPrint("[TestCrash]run Crash7----begin----1")
    if not self.Children then
        local testActor = CreateTestActor(nil, "UnLua.UI.ActorCallback", worldContextObject)
        --TestCrash.RootComponent = testActor.RootComponent
        UEPrint("[TestCrash]run Crash7----begin----2")
        self.Children = testActor.Children
        self.Children:Add(TestCrash.TestActor)
        testActor:K2_DestroyActor()
        UE4.UKismetSystemLibrary.CollectGarbage()  -- 立即GC一下
    else
        self.Children:Add(TestCrash.TestActor)
    end
    UEPrint("[TestCrash]run Crash7")
end

-- 结构体的拷贝与比较
-- ScriptStruct_Compare
-- 结果：没有异常，但结果不对
function TestCrash:Crash8(worldContextObject)
    local d1 = UE4.FDolphinUpdateInfo()
    local d2 = UE4.FDolphinUpdateInfo()
    local c = UE4.FColor(0, 1, 2)
    local trans = CreateDefaultTransform() -- 用于C++下断点，没有特别意义
    local bSame = d1 == c
    UEPrint("[TestCrash]run Crash8")
end

-- 引用结构体成员，并且保存，下次再使用
-- 结果：延迟Crash
function TestCrash:Crash9(worldContextObject)
    if self.AvatarParams == nil then
        local AvatarClass = UE4.UFCTest
        local obj = NewObject(AvatarClass)	
        self.AvatarParams = obj.AvatarParams
    end
    local HideBoneWhiteList = self.AvatarParams.HideBoneWhiteList
    HideBoneWhiteList:Add("abc")
    UEPrint("[TestCrash]run Crash9")
end

-- 调用C++接口，传错误的参数
function TestCrash:Crash10(worldContextObject)
    if self.TestB == nil then
        self.TestB = NewObject(UE4.UFCTestB)
        self.TestObj = NewObject(UE4.UFCTest)
        UEPrint("[TestCrash]run Crash10, TestB=", self.TestB, ",TestObj=", self.TestObj)
    end
    local testB = self.TestB
    local testObj = self.TestObj
    testB:SetBasePtr(testObj, testObj)
    testB.SetPtr(testObj, testObj)
    testObj:SetActor(testObj)
    UEPrint("[TestCrash]run Crash10")
end

-- 调用C++接口，传错误的Struct参数
function TestCrash:Crash11(worldContextObject)
    local testB = NewObject(UE4.UFCTestB)    
    local param = UE4.FTestBoneAdjustItemInfo()
    testB:SetAvatarParam(param)
    UEPrint("[TestCrash]run Crash11, testB=", testB, ",param=", param)
end

-- 结构体拷贝，类型不一致, 内存写坏，很隐秘，会破坏虚拟机, 出现奇怪的异常
function TestCrash:Crash12(worldContextObject)
    local A = UE4.FTransform()
    local B = UE4.FVector2D()
    A:Copy(B)
    UEPrint("[TestCrash]run Crash12, A=", A, ",B=", B)
    A:CopyFrom(B)
    UEPrint("[TestCrash]run Crash12, A=", A, ",B=", B)
end

-- 测试参数TArray, 会不会有内存泄漏
function TestCrash:Crash13(worldContextObject)
    local nStart = os.clock()
    local AvatarClass = UE4.UFCTest
    local obj = NewObject(AvatarClass)    
	local a = UE4.TArray(UE4.int32)
    for i = 1, 10000 do
        a:Add(i)
    end
	obj:SetIDList(a)
    
	local b = UE4.TArray(UE4.int32)
    b:Add(1)
    b:Add(2)
    for i = 1, 1000 do
        obj:GetIDList(b)
    end    
    local costTime = os.clock() - nStart
    UEPrint("[TestCrash]run Crash13, cost time:", costTime)
end

-- 测试C++调用lua overiden函数，参数是引用类型, 引用是不是能生效
function TestCrash:Crash14(worldContextObject)
    -- local AvatarClass = UE4.UClass.Load("UFCTest_C")
    local AvatarClass = UE4.UFCTest
    local obj = NewObject(AvatarClass, worldContextObject, "FCTestIns", "UnLua.UI.ActorCallback")
    obj:TestCall_NotifyIDList()
    obj:TestCall_NotifyAvatarParam()
    obj:TestCall_NotifyIDSet()
    obj:TestCall_NotifyIDMap()
    UEPrint("[TestCrash]run Crash14")
end

-- 测试默认参数
function TestCrash:Crash15(worldContextObject)
    local AvatarClass = UE4.UFCTest
    local obj = NewObject(AvatarClass, worldContextObject, "FCTestIns", "UnLua.UI.ActorCallback")
    obj:TestCall_DefaultParam(80)

    local Set = UE4.TSet(UE4.int32)
    Set:Add(100)
    Set:Add(102)
    Set:Add(302)
    for k, v in pairs(Set) do
        UEPrint("[TestCrash]k =", k, ",v=", v)
    end
end

-- 只是测试DataTable的功能
function TestCrash:Crash16(worldContextObject)
    local classType = UE4.UDataTable
    local dataTable = LoadObject("/Game/TestDataTable.TestDataTable")
    UEPrint("[Crash16]dataTable=", dataTable, ",classType=", classType)
    local OutRowNames = UE4.TArray(UE4.FName)
    UE4.UDataTableFunctionLibrary.GetDataTableRowNames(dataTable, OutRowNames)
    -- for i = 1, #OutRowNames do
    --     local rowName = OutRowNames[i]
    --     local rowData = dataTable:GetRow(rowName)        
    --     UEPrint("[Crash16]dataTable,[]", rowName, ",rowData=", rowData, ",row ID=", rowData.ID, ",Name=", rowData.Name)
    -- end
    for k, v in pairs(dataTable) do
        UEPrint("[Crash16]dataTable,[]key", k, ",rowData=", v, ",row ID=", v.ID, ",Name=", v.Name)
    end
    local row = dataTable["Row1"]
    UEPrint("[Crash16]dataTable,row=", row, ",row ID=", row.ID, ",Name=", row.Name)
end

-- 只是测试GetSubSystem()
function TestCrash:Crash17(worldContextObject)
    local subSystem = UE4.USubsystemBlueprintLibrary.GetEngineSubsystem(UE4.UAssetTagsSubsystem)
    UEPrint("[Crash17]subSystem:", subSystem)

    local AvatarClass = UE4.UFCTest
    local obj = NewObject(AvatarClass)
    obj.TSubclassOfVar = subSystem
    obj.TSubclassOfTest = obj
    -- obj.WeakPtr = obj
    obj.LazyPtr = obj
    local WeakPtr = UE4.TWeakObjectPtr(obj)
    obj.WeakPtr = WeakPtr
    obj:CallClicked()
    obj.ResPtr = "/Game/TestDataTable.TestDataTable"
    local asetName0 = obj.ResPtr:GetAssetName()
    UEPrint("[Crash17]asetName0:", asetName0)

    local ResPtr = UE4.TSoftObjectPtr("/Game/TestDataTable.TestDataTable")
    obj.ResPtr = ResPtr
    local ClassPtr = UE4.TSoftClassPtr(AvatarClass)
    obj.TSoftClassPtrVar = ClassPtr
    ResPtr:LoadSynchronous()
    ClassPtr:LoadSynchronous()
    obj:SetSoftPtr(ResPtr, ClassPtr)
    local assetName1 = ResPtr:GetAssetName()
    local assetName2 = ClassPtr:GetAssetName()
    UEPrint("[Crash17]assetName1:", assetName1, "assetName2:", assetName2)
end

-- 只是测试功能
function TestCrash:Crash18(worldContextObject)
    -- Blueprint'/Game/TestActorBP.TestActorBP'    
    local actor = CreateTestActor("/Game/TestActorBP.TestActorBP", "UnLua.UI.ActorCallback", worldContextObject)
    actor:Died_Brocast(100)
end

function  TestCrash:DoCrash(worldContextObject)
    local nextFuncIndex = self.NextFuncIndex or 0
    nextFuncIndex = nextFuncIndex + 1
    if nextFuncIndex > 9 then
        nextFuncIndex = 1
    end
    self.NextFuncIndex = nextFuncIndex
    local funcName = "Crash" .. nextFuncIndex
    local func = self[funcName]
    -- func(self, worldContextObject)
    -- self:Crash1(worldContextObject) -- 正常
    -- self:Crash2(worldContextObject) -- 修改C++后正常
    -- self:Crash3(worldContextObject) -- 会Crash， 但第二次测试又不会了
    -- self:Crash4(worldContextObject) -- 正常
    -- self:Crash5(worldContextObject) -- 会Crash
    -- self:Crash6(worldContextObject) -- 会Crash
    -- self:Crash7(worldContextObject) -- 第二次执行Crash
    -- self:Crash8(worldContextObject) -- 会Crash
    -- self:Crash9(worldContextObject) -- 会延迟Crash
    -- self:Crash10(worldContextObject) -- 会Crash
    -- self:Crash11(worldContextObject) -- 正常
    -- self:Crash12(worldContextObject) -- 多次执行+反复GC后 Crash
    -- self:Crash13(worldContextObject) -- 正常
    -- self:Crash14(worldContextObject) --会Crash, 功能不正确
    -- self:Crash15(worldContextObject) --
    -- self:Crash16(worldContextObject) --
    -- self:Crash17(worldContextObject) --
    self:Crash18(worldContextObject) --

    ----- 下面是FUnLua的测试结果
    -- self:Crash1(worldContextObject) -- 正常
    -- self:Crash2(worldContextObject) -- 正常
    -- self:Crash3(worldContextObject) -- 正常
    -- self:Crash4(worldContextObject) -- 正常
    -- self:Crash5(worldContextObject) -- 正常
    -- self:Crash6(worldContextObject) -- 正常
    -- self:Crash7(worldContextObject) -- 正常
    -- self:Crash8(worldContextObject) -- 正常
    -- self:Crash9(worldContextObject) -- 正常
    -- self:Crash10(worldContextObject) -- 正常
    -- self:Crash11(worldContextObject) -- 正常
    -- self:Crash12(worldContextObject) -- 正常
    -- self:Crash13(worldContextObject) -- 正常
    -- self:Crash14(worldContextObject) -- 正常
end

return TestCrash
