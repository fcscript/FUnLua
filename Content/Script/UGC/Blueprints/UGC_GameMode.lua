-- UGC_GameMode
local M = {}
local UGC_DragInfo = require("UGC.Operator.UGC_DragInfo")
local UGC_OperatorType = require("UGC.Operator.UGC_OperatorType")
local UGC_SceneObjects = require("UGC.Operator.UGC_SceneObjects")
local UGC_SelectInfo = require("UGC.Operator.UGC_SelectInfo")
local UGC_EventManager = require("UGC.Event.UGC_EventManager")

_G.UGC = 
{
    DragInfo = UGC_DragInfo,
    OperatorType = UGC_OperatorType,
    SceneObjects = UGC_SceneObjects,
    SelectInfo = UGC_SelectInfo,
    EventManager = UGC_EventManager,
}

function M:ReceiveBeginPlay()
    local World = self:GetWorld()
    local GameInstance = World.OwningGameInstance
    print("[UGC]ReceiveBeginPlay, Name:", self:GetName(), ",World=", World, ",GameInstance=", GameInstance)

    local UGC_OperatorMrg = require("UGC.Operator.UGC_OperatorMrg")
    _G.UGC.OperatorMrg = UGC_OperatorMrg
    UGC_OperatorMrg:Init(GameInstance)
    
    -- 显示主UI
    local BPClassPath = "/Game/UGC/UI/W_UGC_Main.W_UGC_Main_C"
	local Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load(BPClassPath))
	Widget:AddToViewport()
    
    -- BPClassPath = "/Game/UGC/UI/W_TestScroll.W_TestScroll_C"
	-- Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load(BPClassPath))
	-- Widget:AddToViewport()
    
    BPClassPath = "/Game/UGC/UI/W_TestMovePanel.W_TestMovePanel_C"
	Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load(BPClassPath))
	Widget:AddToViewport()
end

function M:ReceiveEndPlay(EndPlayReason)
    print("[UGC]ReceiveEndPlay, Name:", self:GetName(), ",EndPlayReason=", EndPlayReason)
end

return M