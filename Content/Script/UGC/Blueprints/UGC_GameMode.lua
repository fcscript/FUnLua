-- UGC_GameMode
local M = {}

function M:ReceiveBeginPlay()
    print("[UGC]ReceiveBeginPlay, Name:", self:GetName())
    
    -- 显示主UI
    local BPClassPath = "/Game/UGC/UI/W_UGC_Main.W_UGC_Main_C"
	local Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load(BPClassPath))
	Widget:AddToViewport()
    
    BPClassPath = "/Game/UGC/UI/W_TestScroll.W_TestScroll_C"
	Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load(BPClassPath))
	Widget:AddToViewport()
    
    BPClassPath = "/Game/UGC/UI/W_TestMovePanel.W_TestMovePanel_C"
	Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load(BPClassPath))
	Widget:AddToViewport()
end

function M:ReceiveEndPlay(EndPlayReason)
    print("[UGC]ReceiveEndPlay, Name:", self:GetName(), ",EndPlayReason=", EndPlayReason)
end

return M