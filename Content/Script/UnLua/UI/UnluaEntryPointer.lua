require "UnLua.UnLua"

-- CMShow.GitCMShow.Script.UI.UnluaEntryPointer
UEPrint("[Unlua]UnluaEntryPointer is required")
local UnluaEntryPointer = LuaUnrealClass()

function UnluaEntryPointer:Construct()
    UEPrint("[Unlua]UnluaEntryPointer:Construct")    
end

function UnluaEntryPointer:Destruct()
    UEPrint("[Unlua]UnluaEntryPointer:Destruct")
end

function UnluaEntryPointer:ReceiveBeginPlay()
    UEPrint("[Unlua]UnluaEntryPointer:ReceiveBeginPlay")
    local AvatarClass = UClass.Load("UFCTest")
    local obj = NewObject(AvatarClass)
    obj.ID = 10
    local ID = obj.ID
    
    -- local a = TArray(_G.FString)
    -- a:Add("aaa")
    -- a:Add("aaa")

	local map = TMap(_G.int32, _G.int32)
	map:Add(1, 10)
	map:Add(2, 20)
	obj:GetIDList(map)
	-- obj:SetIDMap(a)

    local world = self:GetWorld()
    local UIManager = require "UnLua.UI.UIManager"
	local ClassName = "/Game/UnLua/UI/UMG/UMG_MainPanel"    
    UIManager.OpenPanel(world, ClassName)    
end

return UnluaEntryPointer