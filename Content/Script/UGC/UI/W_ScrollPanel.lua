-- W_ScrollPanel
local M = {}

function M:OnDragStarted(MyGeometry, InTouchEvent)    
    local gemetry = MyGeometry
    
    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(InTouchEvent)  -- 取屏幕坐标
    local panelPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(gemetry, screenPos)  -- 转换成本地坐标
    local localSize = UE4.USlateBlueprintLibrary.GetLocalSize(gemetry) -- 取本地Panel的大小
    local localPos = localSize - panelPos   -- 转换成Panel相对坐标
    
    self.bDraging = true
    self.StartPanelPos = localPos
    self.StartScreenPos = screenPos

    self.PanelSize = UE4.USlateBlueprintLibrary.GetLocalSize(MyGeometry) -- 取本地Panel的大小
        
    print("[UGC]StartScrollbarPos=", self.StartScrollbarPos, ",PanelSize=", self.PanelSize)
end

function M:OnDragMoving(MyGeometry, InTouchEvent)
    if not self.bDraging then
        return 
    end
    local gemetry = MyGeometry

    local mainRootGemetry = MyGeometry
    local vecLU = UE.FVector2D(0, 0)
    local selfScreenPos = UE4.USlateBlueprintLibrary.LocalToAbsolute(mainRootGemetry, vecLU)
    
    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(InTouchEvent)  -- 取屏幕坐标
    local panelPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(gemetry, screenPos)  -- 转换成本地坐标
    local localSize = UE4.USlateBlueprintLibrary.GetLocalSize(gemetry) -- 取本地Panel的大小
    local localPos = localSize - panelPos   -- 转换成Panel相对坐标
    
    local moveY = (localPos.Y - self.StartPanelPos.Y) / (self.PanelSize.Y + 0)

    -- 真实的屏幕坐标 = 当前屏幕坐标 - 左上角的屏幕坐标

    -- 滚动条的范围 = (单个节点的高度 * 总的节点的数 - 列表的高度) / 单个节点的高度
    -- print("[UGC]localPos:", localPos, ",screenPos:", screenPos, ",selfScreenPos:", selfScreenPos)
    -- 小于0，就是超出窗口了    
    print("[UGC]moveY=", moveY)
end

function M:OnDragEnd(MyGeometry, InTouchEvent)
    self.bDraging = false
end

function M:OnTouchStarted(MyGeometry, InTouchEvent)
    print("[UGC][OnTouchStarted]InTouchEvent,", InTouchEvent)
    self:OnDragStarted(MyGeometry, InTouchEvent)
end

function M:OnTouchMoved(MyGeometry, InTouchEvent)
    self:OnDragMoving(MyGeometry, InTouchEvent)
end

function M:OnTouchEnded(MyGeometry, InTouchEvent)
    self:OnDragEnd(MyGeometry, InTouchEvent)
end

function M:OnMouseButtonDown(MyGeometry, MouseEvent)
    print("[UGC][OnMouseButtonDown]MouseEvent,", MouseEvent)
    self:OnDragStarted(MyGeometry, MouseEvent)

    -- return UE.UWidgetBlueprintLibrary.Unhandled()
    return UE.UWidgetBlueprintLibrary.Handled()
end

function M:OnMouseMove(MyGeometry, MouseEvent)    
    self:OnDragMoving(MyGeometry, MouseEvent)
    -- return UE.UWidgetBlueprintLibrary.Unhandled()
    return UE.UWidgetBlueprintLibrary.Handled()
end

function M:OnMouseButtonUp(MyGeometry, MouseEvent)
    print("[UGC][OnMouseButtonUp]")
    self:OnDragEnd(MyGeometry, MouseEvent)
    -- return UE.UWidgetBlueprintLibrary.Unhandled()
    return UE.UWidgetBlueprintLibrary.Handled()
end


return M