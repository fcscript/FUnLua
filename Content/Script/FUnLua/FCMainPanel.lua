local FCMainPanel = {}
local TestCrash = require "UnLua.UI.UnLuaCrashTest"

function FCMainPanel:ReceiveBeginPlay()
	print("[FCTestScript]FCMainPanel:ReceiveBeginPlay")
	self.Button_0.OnClicked:AddListener(FCMainPanel.OnButtonClicked, self)	
	self.Button.OnClicked:AddListener(FCMainPanel.OnGCButtonClicked, self)	
	self.Button_1.OnClicked:AddListener(FCMainPanel.OnChangeMapButtonClicked, self)	
	self.Button_Print.OnClicked:AddListener(FCMainPanel.PrintRefInfo, self)	
	self.ButtonProfile.OnClicked:AddListener(FCMainPanel.OnButtonProfileClicked, self)	
	self.ButtonCrash.OnClicked:AddListener(FCMainPanel.OnButtonCrashClicked, self)	
	self.ButtonCppCallLua.OnClicked:AddListener(FCMainPanel.OnButtonCppCallLuaClicked, self)	
	local world = self:GetWorld()
	local localPlayerControler = _G.UGameplayStatics.GetPlayerController(world, 0)
	if localPlayerControler ~= nil then
		localPlayerControler.bShowMouseCursor = 1
	end
	print("[FCTestScript]FCMainPanel:ReceiveBeginPlay, world=", world, ",localPlayerControler=", localPlayerControler)
	self.MapName:SetText(world:GetName())
end

function FCMainPanel:ReceiveBeginDestroy()
	print("[FCTestScript]FCMainPanel:ReceiveBeginDestroy")
end

function FCMainPanel:OnButtonClicked()
	print("[FCTestScript]FCMainPanel:OnButtonClicked")
	local world = self:GetWorld()
	local  ClassName = "/Game/UMG/UMG_TestPanel.UMG_TestPanel_C"
	local UIManager = require "FUnLua.UIManager"
	UIManager.OpenPanel(world, ClassName)
end

function FCMainPanel:OnGCButtonClicked()
	collectgarbage("collect")
    _G.UKismetSystemLibrary.CollectGarbage()  -- 立即GC一下
	print("[FCTestScript]FCMainPanel:OnGCButtonClicked")
	self:PrintRefInfo()
end

function FCMainPanel:OnButtonProfileClicked()
	print("[FCTestScript]FCMainPanel:OnButtonProfileClicked")
	local world = self:GetWorld()
	local ClassName = "/Game/UMG/UMG_Profile.UMG_Profile_C"
	local UIManager = require "FUnLua.UIManager"
	UIManager.OpenPanel(world, ClassName)
end

function FCMainPanel:OnButtonCrashClicked()
	local world = self:GetWorld()
    TestCrash:DoCrash(world)
end

local function PrintMap(Map)
	for key, value in pairs(Map) do
		print("[PrintMap]map,key=", key, ",value=", value)
	end
end

function _G.PrintStructList(List)
	print("[PrintStructList]--------begin, List=", List)
	for i = 1, #List do
		print("[PrintMap]List, i=", i, ",value=", List[i])
	end
end

function FCMainPanel:OnButtonCppCallLuaClicked()	
    local AvatarClass = UClass.Load("UFCTest")
    local obj = NewObject(AvatarClass)
	obj.bUseFlag2 = 1
	obj.bUseFlag1 = 1
	obj:CheckBool()
	obj.bUseFlag1 = 1
	obj:CheckBool()

	-- local Map = UE.TMap(_G.int32, _G.FString)
	-- Map:Add(1, "a1")
	-- Map:Add(2, "a2")
	-- Map:Add(3, "a3")
	-- PrintMap(Map)
	-- UE.UFCTest.CallMapParamFunc("MainPanel_TestMap", Map)
	
	-- UE.UFCTest.CallStringMapFunc("MainPanel_TestMap", Map) -- 参数错了，会Crash

	-- local StrMap = UE.TMap(_G.FString, _G.int32)
	-- StrMap:Add("a1", 1)
	-- StrMap:Add("a2", 2)
	-- StrMap:Add("a3", 3)
	-- UE.UFCTest.CallStringMapFunc("MainPanel_TestMap", StrMap)
	
    -- local Set = UE4.TSet(UE4.int32)
    -- Set:Add(100)
    -- Set:Add(102)
    -- Set:Add(302)
	-- UE.UFCTest.CallSetParamFunc("MainPanel_TestMap", Set)

	-- local Array = UE4.TArray(UE4.FString)
	-- Array:Add("t1")
	-- Array:Add("t2")
	-- Array:Add("t3")
	-- UE.UFCTest.CallTArrayParamFunc("MainPanel_TestMap", Array)

	-- local Array = UE4.TArray(_G.int32)
	-- Array:Add(1)
	-- Array:Add(2)
	-- Array:Add(3)
	-- UE.UFCTest.CallTArrayParamFunc("MainPanel_TestMap", Array)

	-- local Array = UE4.TArray(UE4.FVector)
	-- Array:Add(UE4.FVector(1,2,3))
	-- Array:Add(UE4.FVector(2,3,4))
	-- Array:Add(UE4.FVector(4,5,6))
	-- UE.UFCTest.CallTArraVectorFunc("MainPanel_TestMap", Array)

	-- local Array = UE4.TArray(UE4.FTestBoneAdjustItemInfo)
	-- Array:Add(UE4.FTestBoneAdjustItemInfo())
	-- Array:Add(UE4.FTestBoneAdjustItemInfo())
	-- Array:Add(UE4.FTestBoneAdjustItemInfo())
	-- print("before call, #Array=", #Array)
	-- UE.UFCTest.CallTArrayTestItemInfo("PrintStructList", Array)
end

function _G.MainPanel_TestMap(Map)
	print("[MainPanel_TestMap]--------begin")
	PrintMap(Map)
end

function FCMainPanel:PrintRefInfo()
	local world = self:GetWorld()
	local TotalBindCount = _G.GetBindObjectCount()
	local TotalRef = _G.GetTotalObjRef()
	local TotalIntPtr = _G.GetTotalIntPtr()
	local StrLog = "Total bind object:" .. TotalBindCount .. ",RefObj:" .. TotalRef .. ",IntPtr:" .. TotalIntPtr
	local color = FLinearColor()
	color.A = 1	
	color.G = 1
	UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
end


function FCMainPanel:OnChangeMapButtonClicked()
	local world = self:GetWorld()
	local worldName = world:GetName()
	local nextMapName = "OtherMap"
	if worldName == "OtherMap" then
		nextMapName = "EmptyMap"
	end
	self:PrintRefInfo()
	UE4.UGameplayStatics.OpenLevel(world, nextMapName, true, "")
	print("[FCTestScript]FCMainPanel:OnChangeMapButtonClicked, curMap:", worldName, ",NextMap:", nextMapName)
end

function FCMainPanel:OnMouseButtonDown(MyGeometry, MouseEvent)
	print("[FCTestScript]OnMouseButtonDown")
	local world = self:GetWorld()
	local localPlayerControler = _G.UGameplayStatics.GetPlayerController(world, 0)
	if localPlayerControler ~= nil then
		localPlayerControler.bShowMouseCursor = 1
	end
	print("[FCTestScript]OnMouseButtonDown, world=", world,  ",localPlayerControler=", localPlayerControler, ",bShowMouseCursor=", localPlayerControler.bShowMouseCursor)
	self:PrintRefInfo()
	return UWidgetBlueprintLibrary.Handled()
end

function FCMainPanel:OnMouseButtonUp(MyGeometry, MouseEvent)
	print("[FCTestScript]OnMouseButtonUp")
	return UWidgetBlueprintLibrary.Handled()
end

return FCMainPanel