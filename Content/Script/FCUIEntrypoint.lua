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
	a:Add(201)
	a:Add(501)

	local map = TMap("int", "int")
	map:Add(1, 10)
	map:Add(2, 20)

	local it = map:begin()
	while(it:IsValid()) do
		print("[FCTestScript]map,key=", it.key, ",value=", it.value)
		it:ToNext()
	end

	obj.IDMap = map
	obj:SetIDMap(map)

	local b = obj:GetIDList()
	-- local len = a:Length()
	print("[FCTestScript]b=", b)
	if b then
		local len = #b
		for i = 1, len do
			print("[FCTestScript]b:", b:Get(i))
		end	
	end

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