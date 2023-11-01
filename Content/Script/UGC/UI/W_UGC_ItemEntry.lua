-- W_UGC_ItemEntry
local M = {}

function M:Construct()
end

function M:Destruct()    
end

function M:OnInitialized()
end

function M:RefreshListEntry(CustomInfo, Item, bSelect)
    print("[UGC]RefreshListEntry, bSelect=", bSelect)
    self.CustomInfo = CustomInfo
    self.Item = Item
    self.ID = Item.ID
    self.LB_ItemName:SetText(Item.Name)
    self:RefreshSelectFlag(bSelect)
end

function M:RefreshSelectFlag(bSelect)
    if bSelect then
        self.Image_Item:SetColorAndOpacity(UE.FLinearColor(0, 0.267361, 1, 1))
    else
        self.Image_Item:SetColorAndOpacity(UE.FLinearColor(1, 1, 1, 1))
    end
end

function M:SelectListEntry(bSelect)
    print("[UGC]SelectListEntry, bSelect=", bSelect, ",ID=", self.ID)
    self:RefreshSelectFlag(bSelect)
end

function M:AutoCreateDragItem(screenPos)
    local UGC_DragInfo = require("UGC.UI.UGC_DragInfo")
    local LineTraceUtil = require("UGC.Util.LineTraceUtil")
    local pickPos = LineTraceUtil:GetPickupPosition(self, screenPos, UGC_DragInfo.WorldItem)

    -- print("[UGC]AutoCreateDragItem, pickPos=", pickPos)

    if UGC_DragInfo.WorldItem == nil then
        local bpPath = self.Item.BpPath
        print("[UGC]bpPath=", bpPath)
        -- bpPath = "/Game/UGC/ItemTemplate/ZhaLan1.ZhaLan1_C"
        local itemBpClass = UE.UClass.Load(bpPath)
        local initRotation = UE.FRotator(0, 0, 0)
        local transform = UE.FTransform(initRotation:ToQuat(), pickPos)
        local moduleName = nil
        local world = self:GetWorld()
        local UGC_SceneObjects = require("UGC.UI.UGC_SceneObjects")

        local ObjNextIDs = UGC_SceneObjects.ObjNextIDs
        local nextID = ObjNextIDs[self.Item.Name] or 0
        nextID = nextID + 1
        ObjNextIDs[self.Item.Name] = nextID

        local objName = self.Item.Name .. "_" .. tostring(nextID)
        UGC_DragInfo.WorldItem = world:SpawnActor(itemBpClass, transform, UE.ESpawnActorCollisionHandlingMethod.AlwaysSpawn, nil, nil, moduleName, nil, nil, objName)
        local SceneObjcts = UGC_SceneObjects.Objects
        SceneObjcts[#SceneObjcts + 1] = UGC_DragInfo.WorldItem
        -- 通知UI刷新, 发送消息
        local UGC_EventManager = require("UGC.Event.UGC_EventManager")
        UGC_EventManager:SendUGCMessage("UGC.Event.AddObject", UGC_DragInfo.WorldItem)
    elseif UE.UObject.IsValid(UGC_DragInfo.WorldItem) then
        local SweepHitResult = UE.FHitResult()
        UGC_DragInfo.WorldItem:K2_GetRootComponent():K2_SetRelativeLocation(pickPos, false, SweepHitResult, false)
    end
end

function M:OnDragStarted(MyGeometry, InTouchEvent)
    _G.GlbSelectListViewItem(self.CustomInfo.ListView, self.Item)
    local ListView = self.CustomInfo.ListView
    local RootPanel = self.CustomInfo.RootPanel
    local gemetry = RootPanel:GetCachedGeometry()
    
    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(InTouchEvent)  -- 取屏幕坐标
    local panelPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(gemetry, screenPos)  -- 转换成本地坐标
    local localSize = UE4.USlateBlueprintLibrary.GetLocalSize(gemetry) -- 取本地Panel的大小
    local localPos = localSize - panelPos   -- 转换成Panel相对坐标
    
    self.bDraging = true
    self.StartPanelPos = localPos
    self.StartScreenPos = screenPos
    self.StartScrollbarPos = ListView:GetScrollOffset()

    self.PanelSize = UE4.USlateBlueprintLibrary.GetLocalSize(MyGeometry) -- 取本地Panel的大小
        
    print("[UGC]StartScrollbarPos=", self.StartScrollbarPos, ",PanelSize=", self.PanelSize)
end

function M:OnDragMoving(MyGeometry, InTouchEvent)
    if not self.bDraging then
        return 
    end
    local UGC_DragInfo = require("UGC.UI.UGC_DragInfo")
    local MainRoot = UGC_DragInfo.MainRoot
    local ListView = self.CustomInfo.ListView
    local RootPanel = self.CustomInfo.RootPanel
    local gemetry = RootPanel:GetCachedGeometry()

    local mainRootGemetry = MainRoot:GetCachedGeometry()
    local vecLU = UE.FVector2D(0, 0)
    local selfScreenPos = UE4.USlateBlueprintLibrary.LocalToAbsolute(mainRootGemetry, vecLU)
    
    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(InTouchEvent)  -- 取屏幕坐标
    local panelPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(gemetry, screenPos)  -- 转换成本地坐标
    local localSize = UE4.USlateBlueprintLibrary.GetLocalSize(gemetry) -- 取本地Panel的大小
    local localPos = localSize - panelPos   -- 转换成Panel相对坐标
    
    local moveY = (localPos.Y - self.StartPanelPos.Y) / (self.PanelSize.Y + ListView.EntrySpacing)
    local scrollbarPos = self.StartScrollbarPos + moveY

    -- 真实的屏幕坐标 = 当前屏幕坐标 - 左上角的屏幕坐标
    local realScreenPos = UE.FVector2D(screenPos.X - selfScreenPos.X, screenPos.Y - selfScreenPos.Y)

    -- 滚动条的范围 = (单个节点的高度 * 总的节点的数 - 列表的高度) / 单个节点的高度
    -- print("[UGC]localPos:", localPos, ",screenPos:", screenPos, ",selfScreenPos:", selfScreenPos)
    -- 小于0，就是超出窗口了    
    if localPos.X < -10 or UGC_DragInfo.WorldItem ~= nil then
        self:AutoCreateDragItem(realScreenPos)
        return
    end

    ListView:SetScrollOffset(scrollbarPos)
end

function M:OnDragEnd(MyGeometry, InTouchEvent)
    self.bDraging = false
    local UGC_DragInfo = require("UGC.UI.UGC_DragInfo")
    UGC_DragInfo.WorldItem = nil
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

    return UE.UWidgetBlueprintLibrary.Unhandled()
end

function M:OnMouseMove(MyGeometry, MouseEvent)    
    self:OnDragMoving(MyGeometry, MouseEvent)
end

function M:OnMouseButtonUp(MyGeometry, MouseEvent)
    print("[UGC][OnMouseButtonUp]")
    self:OnDragEnd(MyGeometry, MouseEvent)
    return UE.UWidgetBlueprintLibrary.Unhandled()
end


return M