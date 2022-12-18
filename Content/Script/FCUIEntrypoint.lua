require "UnLua"
local UIManager = require "UIManager"

local FCUIEntrypoint = {}

function FCUIEntrypoint:ReceiveBeginPlay()	
    local AvatarClass = UClass.Load("UFCTest")
    local obj = NewObject(AvatarClass)
    local objAvatar = obj.AvatarParams	
	objAvatar.BoneAdjustItem.SlotName = "abc"

	local a = TArray("int")
	a:Add(1)
	a:Add(2)
	-- local len = a:Length()
	local len = #a
	for i = 1, len do
		print("[FCTestScript]a:", a:Get(i))
	end
	
	local b = TArray("int")
	b:Add(1)
	b:Add(2)

	local Name = self:GetName()
	local world = self:GetWorld()
	local worldName = world:GetName()
	UIManager.Reset(world)	
	print("[FCTestScript]FCUIEntrypoint:ReceiveBeginPlay-----, self=", self, ",Name:", Name, ",worldName:", worldName)
	self:LoadMainPanel()
end

function FCUIEntrypoint:LoadMainPanel()
	local world = self:GetWorld()
	local ClassName = "/Game/UMG/UMG_MainPanel.UMG_MainPanel_C"
	print("[FCTestScript][FCUIEntryPoint]LoadUserWidget, ClassName=", ClassName)
	UIManager.OpenPanel(world, ClassName)
end

return FCUIEntrypoint