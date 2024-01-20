---@class DragScrollPanelBase
local DragScrollPanelBase = {}

function DragScrollPanelBase:OnTouchStarted(MyGeometry, InTouchEvent)
    print("[UIClick][MovePanel]InTouchEvent,", InTouchEvent)
    return self:TryDragStarted(MyGeometry, MouseEvent)
end

function DragScrollPanelBase:OnTouchMoved(MyGeometry, InTouchEvent)
    self:OnDragMoving(MyGeometry, InTouchEvent)
    return UE.UWidgetBlueprintLibrary.Handled()
end

function DragScrollPanelBase:OnTouchEnded(MyGeometry, InTouchEvent)
    self:OnDragEnd(MyGeometry, InTouchEvent)
    return UE.UWidgetBlueprintLibrary.Handled()
end

function DragScrollPanelBase:OnMouseButtonDown(MyGeometry, MouseEvent)
    print("[UIClick][MovePanel]OnMouseButtonDown, MouseEvent", MouseEvent)
    return self:TryDragStarted(MyGeometry, MouseEvent)
end

function DragScrollPanelBase:OnMouseMove(MyGeometry, MouseEvent)    
    self:OnDragMoving(MyGeometry, MouseEvent)
    return UE.UWidgetBlueprintLibrary.Handled()
end

function DragScrollPanelBase:OnMouseButtonUp(MyGeometry, MouseEvent)
    print("[UIClick][MovePanel][OnMouseButtonUp]")
    self:OnDragEnd(MyGeometry, MouseEvent)
    return UE.UWidgetBlueprintLibrary.Handled()
end

function DragScrollPanelBase:TryDragStarted(MyGeometry, MouseEvent)    
    local bInRange = self.MovingPanel:IsDragRange(MyGeometry, MouseEvent)
    print("[UIClick][MovePanel]TryDragStarted,", MouseEvent, ",bInRange=", bInRange)    
    if bInRange then
		self:ShowDragImage(true)
        self:OnDragStarted(MyGeometry, MouseEvent)
        return UE.UWidgetBlueprintLibrary.Handled()
    else
        return UE.UWidgetBlueprintLibrary.Unhandled()
    end
end

function DragScrollPanelBase:OnDragStarted(MyGeometry, MouseEvent)
    self.MovingPanel:OnDragStart(MyGeometry, MouseEvent)
end

function DragScrollPanelBase:OnDragMoving(MyGeometry, MouseEvent)
    self.MovingPanel:OnDragging(MyGeometry, MouseEvent)
end

function DragScrollPanelBase:OnDragEnd(MyGeometry, MouseEvent)
	self:ShowDragImage(false)
    self.MovingPanel:OnDragEnd(MyGeometry, MouseEvent)
end

function DragScrollPanelBase:Tick(MyGeometry, InDeltaTime)
    self.MovingPanel:UpdateMove(InDeltaTime)
end

function DragScrollPanelBase:OnLeaveItem(ChildWiget, Index)
    -- print("DragScrollPanelBase:OnLeaveItem(), Index=", Index)
end

function DragScrollPanelBase:OnEnterItem(ChildWiget, Index)
    -- print("DragScrollPanelBase:OnEnterItem(), Index=", Index)
end

function DragScrollPanelBase:ShowDragImage(bShow)
	if self.Image_DragBk then
		if bShow then
			self.Image_DragBk:SetVisibility(UE4.ESlateVisibility.Visible)
		else
			self.Image_DragBk:SetVisibility(UE4.ESlateVisibility.Hidden)
		end
	end
end


return DragScrollPanelBase