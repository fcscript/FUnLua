-- require "FUnLua.UnLua"
local UIManager = require "FUnLua.UIManager"

local FCUIEntrypoint = {}

function FCUIEntrypoint:ReceiveBeginPlay()	
	print("[FCTestScript]ReceiveBeginPlay")
    local AvatarClass = UClass.Load("UFCTest")
	print("[FCTestScript]ReceiveBeginPlay 1111")
    local obj = UE4.NewObject(AvatarClass)
    local objAvatar = obj.AvatarParams	
	objAvatar.BoneAdjustItem.SlotName = "abc"
	print("[FCTestScript]ReceiveBeginPlay 12222")

	local v = FVector(1, 2, 3)
	local vecList = TArray("FVector")
	vecList:Add(v)
	vecList:Add(v)
	vecList:Add(v)

	local v1 = vecList:Get(1)
	v1.X = 100
	print("[FCTestScript]v1=", v1, ",[1]:", vecList:Get(1))

	local a = TArray("int")
	a:Add(1)
	a:Add(2)
	a:Add(201)
	a:Add(501)

	local len = #a
	print("[FCTestScript]len=", len)
	for k, v in pairs(a) do
		print("[FCTestScript]k=", k, ",v=", v)
	end

	local objRefMemSize = GetObjRefSize()
	print("[FCTestScript]ObjRef MemSize:", objRefMemSize, ",UFCTest class mem size:", GetClassDescMemSize("UFCTest"))

	-- local map = obj.IDMap
	local map = TMap("int32", "int32")
	map:Add(1, 10)
	map:Add(2, 20)

	local it = map:begin()
	while(it:IsValid()) do
		print("[FCTestScript]map,key=", it.key, ",value=", it.value)
		it:ToNext()
	end

	obj:SetIDList(a)
	-- obj.IDMap = map
	obj:SetIDMap(map)

	local idList1 = TArray(UE4.int32)
	idList1:Add(10)
	idList1:Add(20)
	local b, idList = obj:GetIDList(idList1)
	-- local len = a:Length()
	print("[FCTestScript]b=", b, ",#idList=", #idList, ",idList1=", idList1, "#idList1=", #idList1)
	if b and idList1 then
		local len = #idList1
		for i = 1, len do
			print("[FCTestScript]idList1:", idList1:Get(i))
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