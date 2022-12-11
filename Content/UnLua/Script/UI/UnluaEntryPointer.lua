
require "UnLua.Script.UnLua"
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
    local world = self:GetWorld()
    local UIManager = require "UnLua.Script.UI.UIManager"
	local ClassName = "/Game/UnLua/UI/UMG/UMG_MainPanel"    
    UIManager.OpenPanel(world, ClassName)
end

return UnluaEntryPointer