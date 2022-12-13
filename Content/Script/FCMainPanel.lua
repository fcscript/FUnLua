local FCMainPanel = {}

function FCMainPanel:ReceiveBeginPlay()
	print("[FCTestScript]FCMainPanel:ReceiveBeginPlay")
	self.Button_0.OnClicked:AddListener(FCMainPanel.OnButtonClicked, self)	
	self.Button.OnClicked:AddListener(FCMainPanel.OnGCButtonClicked, self)	
	self.Button_1.OnClicked:AddListener(FCMainPanel.OnChangeMapButtonClicked, self)	
	self.Button_Print.OnClicked:AddListener(FCMainPanel.PrintRefInfo, self)	
	self.ButtonProfile.OnClicked:AddListener(FCMainPanel.OnButtonProfileClicked, self)	
	local world = self:GetWorld()
	local localPlayerControler = _G.UGameplayStatics.GetPlayerController(world, 0)
	if localPlayerControler ~= nil then
		localPlayerControler.bShowMouseCursor = 1
	end
	self.MapName:SetText(world:GetName())
	print("[FCTestScript]FCMainPanel:ReceiveBeginPlay, world=", world, ",localPlayerControler=", localPlayerControler)
end

function FCMainPanel:ReceiveBeginDestroy()

end

function FCMainPanel:OnButtonClicked()
	print("[FCTestScript]FCMainPanel:OnButtonClicked")
	local world = self:GetWorld()
	local  ClassName = "/Game/UMG/UMG_TestPanel.UMG_TestPanel_C"
	local UIManager = require "UIManager"
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
	local UIManager = require "UIManager"
	UIManager.OpenPanel(world, ClassName)
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