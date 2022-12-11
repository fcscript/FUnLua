
local FCTestPanel = {}

function FCTestPanel:ReceiveBeginPlay()
	print("[FCTestScript]FCTestPanel:ReceiveBeginPlay, self=", self, ",__TableAddr=", self.__TableAddr)
	self.ButtonClose.OnClicked:AddListener(FCTestPanel.OnButtonCloseClicked, self)
	self.Button_0.OnClicked:AddListener(FCTestPanel.OnButton1Clicked, self)
	self.Button_1.OnClicked:AddListener(FCTestPanel.OnButton2Clicked, self)
	self.ButtonLuaGC.OnClicked:AddListener(FCTestPanel.OnButtonLuaGCClicked, self)
	self.ButtonUEGC.OnClicked:AddListener(FCTestPanel.OnButtonUEGCClicked, self)
	print("[FCTestScript]FCTestPanel:ReceiveBeginPlay, suc...")
end

function FCTestPanel:ReceiveBeginDestroy()
	print("[FCTestScript]FCTestPanel:ReceiveBeginDestroy")
end

function FCTestPanel:OnButtonCloseClicked()
	local clickCount = self.ClickCount or 0
	print("[FCTestScript]FCTestPanel:OnButtonCloseClicked, self=", self, ",__TableAddr=", self.__TableAddr, ",clickCount=", clickCount)
	self.bVisiable = true
	self.ClickCount = clickCount + 1
	local UIManager = require "UIManager"
	UIManager.HidePanel(self)
end

function FCTestPanel:OnButton1Clicked()
	local Name = self:GetName()
	local clickCount = self.ClickCount or 0
	print("[FCTestScript]FCTestPanel:OnButton1Clicked, self=", self, ",__TableAddr=", self.__TableAddr, ",clickCount=", clickCount)
	self:CallBlueprintFunc1("Input Test Name, cutTime:" .. os.date())
	self.bVisiable = true
	self.ClickCount = clickCount + 1
end

function FCTestPanel:OnButton2Clicked()
	print("[FCTestScript]FCTestPanel:OnButton2Clicked")
	self.ID = self.ID + 1
	self.Type = "Type" .. self.ID
	-- print("[FCTestScript]FCTestPanel:OnButton2Clicked, ID=", self.ID, ",Type=", self.Type)
	self:CallBlueprintFunc2()
	-- self.Overridden:OnButtonEvent2(self.ID, self.Type)
end

function FCTestPanel:OnButtonLuaGCClicked()
	local startTime = os.clock()
	collectgarbage("collect")
	local costTime = os.clock() - startTime
	print("[FCTestScript]FCTestPanel:OnButtonLuaGCClicked, GC cost time:", costTime)
end

function FCTestPanel:OnButtonUEGCClicked()
    _G.UKismetSystemLibrary.CollectGarbage()  -- 立即GC一下
end

function FCTestPanel:OnMouseButtonDown(MyGeometry, MouseEvent)
	print("[FCTestScript]FCTestPanel:OnMouseButtonDown");
	local world = self:GetWorld()
	local localPlayerControler = UGameplayStatics.GetPlayerController(world, 0)
	if localPlayerControler ~= nil then
		localPlayerControler.bShowMouseCursor = 1
	end
	return UWidgetBlueprintLibrary.Handled()
end

function FCTestPanel:OnButtonEvent1(Name)
	print("[FCTestScript]FCTestPanel:OnButtonEvent1, Name:", Name)
end

function FCTestPanel:OnButtonEvent2(ID, Type)
	print("[FCTestScript]FCTestPanel:OnButtonEvent2, ID=", ID, ",Type=", Type)
	self.Overridden:OnButtonEvent2(ID, Type)
end

return FCTestPanel