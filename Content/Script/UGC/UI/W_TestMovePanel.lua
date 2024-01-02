-- W_TestMovePanel
local MovingPanelUtil = require("UGC.Util.MovingPanelUtil")
local M = {}

function M:Construct()
    print("[UGC][MovePanel]:Construct, start, MovingPanelUtil=", MovingPanelUtil)
    self.TickDeltaTime = 0
    self.DragUtil = MovingPanelUtil.New()
    local ItemClassPath = "/Game/UGC/UI/W_ScrollItem.W_ScrollItem"
    local MovingPanel = self.W_MovePanel
    self.DragUtil:Init(self, MovingPanel, MovingPanel.Panel_Childs, ItemClassPath, 3, 200, 200, 100, 0, true)
    local PanelChilds = MovingPanel.Panel_Childs
    print("[UGC][MovePanel]:Construct, end, DragUtil=", self.DragUtil)
    PanelChilds:GetChildAt(0).Image_bk:SetColorAndOpacity(UE.FLinearColor(0.322917, 0.839192, 1, 0.698))
    PanelChilds:GetChildAt(1).Image_bk:SetColorAndOpacity(UE.FLinearColor(0.522917, 0.539192, 1, 0.698))
    PanelChilds:GetChildAt(2).Image_bk:SetColorAndOpacity(UE.FLinearColor(0.822917, 0.339192, 1, 0.698))
    PanelChilds:GetChildAt(0).Label_Index:SetText("1")
    PanelChilds:GetChildAt(1).Label_Index:SetText("2")
    PanelChilds:GetChildAt(2).Label_Index:SetText("3")
end

function M:OnTouchStarted(MyGeometry, InTouchEvent)
    print("[UGC][MovePanel]InTouchEvent,", InTouchEvent)
    return self:TryDragStarted(MyGeometry, MouseEvent)
end

function M:OnTouchMoved(MyGeometry, InTouchEvent)
    self:OnDragMoving(MyGeometry, InTouchEvent)
    return UE.UWidgetBlueprintLibrary.Handled()
end

function M:OnTouchEnded(MyGeometry, InTouchEvent)
    self:OnDragEnd(MyGeometry, InTouchEvent)
    return UE.UWidgetBlueprintLibrary.Handled()
end

function M:OnMouseButtonDown(MyGeometry, MouseEvent)
    print("[UGC][MovePanel]OnMouseButtonDown, MouseEvent", MouseEvent)
    return self:TryDragStarted(MyGeometry, MouseEvent)
end

function M:OnMouseMove(MyGeometry, MouseEvent)    
    self:OnDragMoving(MyGeometry, MouseEvent)
    -- return UE.UWidgetBlueprintLibrary.Unhandled()
    return UE.UWidgetBlueprintLibrary.Handled()
end

function M:OnMouseButtonUp(MyGeometry, MouseEvent)
    print("[UGC][MovePanel][OnMouseButtonUp]")
    self:OnDragEnd(MyGeometry, MouseEvent)
    -- return UE.UWidgetBlueprintLibrary.Unhandled()
    return UE.UWidgetBlueprintLibrary.Handled()
end

function M:TryDragStarted(MyGeometry, MouseEvent)    
    local bInRange = self.DragUtil:IsDragRange(MyGeometry, MouseEvent)
    self.bInRange = bInRange
    print("[UGC][MovePanel]TryDragStarted,", MouseEvent, ",bInRange=", bInRange)    
    if bInRange then
        self:OnDragStarted(MyGeometry, MouseEvent)
        self.Image_back:SetVisibility(UE4.ESlateVisibility.Visible)
        return UE.UWidgetBlueprintLibrary.Handled()
    else
        -- return UE.UWidgetBlueprintLibrary.Handled()
        self.Image_back:SetVisibility(UE4.ESlateVisibility.SelfHitTestInvisible)
        return UE.UWidgetBlueprintLibrary.Unhandled()
    end
end

function M:OnDragStarted(MyGeometry, MouseEvent)
    self.DragUtil:OnDragStart(MyGeometry, MouseEvent)
end

function M:OnDragMoving(MyGeometry, MouseEvent)
    self.DragUtil:OnDragging(MyGeometry, MouseEvent)
end

function M:OnDragEnd(MyGeometry, MouseEvent)
    self.bInRange = false
    self.DragUtil:OnDragEnd(MyGeometry, MouseEvent)
    self.Image_back:SetVisibility(UE4.ESlateVisibility.SelfHitTestInvisible)
end

function M:Tick(MyGeometry, InDeltaTime)
    self.TickDeltaTime = self.TickDeltaTime + InDeltaTime
    self.DragUtil:UpdateMove(InDeltaTime)
    if self.TickDeltaTime > 3 then
        self.TickDeltaTime = 0
    end
end

function M:OnLeaveItem(ChildWiget, Index)
    ChildWiget.Image_Flag:SetColorAndOpacity(UE.FLinearColor(0.687326, 0.843759, 1, 1))
end

function M:OnEnterItem(ChildWiget, Index)
    ChildWiget.Image_Flag:SetColorAndOpacity(UE.FLinearColor(1, 0.915951, 0.126924, 1))
end

-- function M:OnPaint(Context)
--     local DeltaTime = 0.01
--     self.DragUtil:UpdateMove(DeltaTime)
-- end

return M