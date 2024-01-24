-- W_UGC_Backpanel
local M = {}
local LineTraceUtil = require("UGC.Util.LineTraceUtil")

function M:OnMouseButtonDown(MyGeometry, MouseEvent)    
    local vecLU = UE.FVector2D(0, 0)
    local selfScreenPos = UE4.USlateBlueprintLibrary.LocalToAbsolute(MyGeometry, vecLU)

    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    local panelPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(MyGeometry, screenPos)  -- 转换成本地坐标
    -- print("[UGC]panelPos:", panelPos, ",screenPos:", screenPos, ",selfScreenPos:", selfScreenPos)

    -- 真实的屏幕坐标 = 当前屏幕坐标 - 左上角的屏幕坐标
    local realScreenPos = UE.FVector2D(screenPos.X - selfScreenPos.X, screenPos.Y - selfScreenPos.Y)
    local selectActor = self:FindNearActor(realScreenPos)
    local bSelectAxis = _G.UGC.OperatorMrg:TrySelectAxis(MouseEvent)

    if selectActor then
        -- self:SetSelectEffect(selectActor)
        -- UE.USPLuaFunctionLibary.SetActorSelectEffect(selectActor)
        if not bSelectAxis then
            _G.UGC.OperatorMrg:SelectObjectFromScene(selectActor)
        end
    end    
    -- local LineTraceUtil = require("UGC.Util.LineTraceUtil")
    -- local pickPos, hitResult = LineTraceUtil:GetPickupPosition(self, panelPos)
    -- if hitResult then
    --     local HitObjectHandle = hitResult.HitObjectHandle
    --     local hitActor = HitObjectHandle.Actor:Get()
    --     print("[UGC]Find HitResult, hitActor:", hitActor)
    -- end

    local bMouseLD = self:IsLeftMouseDown(MouseEvent)
    self.MouseDownPos = UE.FVector2D(screenPos.X, screenPos.Y)    
    self.bMouseDown = bMouseLD
    self.StartWorldPos = self:ScreenToWorld(screenPos)
    self.StartCameraPos = self:GetCameraPos()
    self.StartPickUpPos = self:GetPickUpPos(MouseEvent)
    self.FirstBoundBoxInfo = _G.UGC.OperatorMrg:GetFirstSelectBoundBoxInfo()
    
    -- print("[UGC]OnMouseButtonDown, bMouseDown=", self.bMouseDown, ",StartCameraPos=", self.StartCameraPos)
    if self.bMouseDown then
        return UE.UWidgetBlueprintLibrary.Handled()
    else
        return UE.UWidgetBlueprintLibrary.Unhandled()
    end
end

function M:OnMouseMove(MyGeometry, MouseEvent)
    local bMouseLD = self:IsLeftMouseDown(MouseEvent)
    if bMouseLD then
        self:OnDraging(MyGeometry, MouseEvent)
        return UE.UWidgetBlueprintLibrary.Handled()
    end
    return UE.UWidgetBlueprintLibrary.Unhandled()
end

function M:OnMouseButtonUp(MyGeometry, MouseEvent)
    local bMouseLD = self:IsLeftMouseDown(MouseEvent)
    self.bMouseDown = false
    if bMouseLD then
        return UE.UWidgetBlueprintLibrary.Handled()
    else
        return UE.UWidgetBlueprintLibrary.Unhandled()
    end
end

function M:IsLeftMouseDown(MouseEvent)
    local Key = UE.FKey()
    Key.KeyName = "LeftMouseButton"
    local bMouseLD = UE.UKismetInputLibrary.PointerEvent_IsMouseButtonDown(MouseEvent, Key)
    return bMouseLD
end

function M:OnDraging(MyGeometry, MouseEvent)    
    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    local WorldPosition = self:ScreenToWorld(screenPos)
    
    -- 如果有选择的对象，并且当前是托动模式    
    if _G.UGC.SelectInfo.Transfrom_Mode ~= _G.UGC.OperatorType.Tf_None then
        local SelectObjects = _G.UGC.SelectInfo.SelectObjects
        if #SelectObjects > 0 then
            self:OnTransformSelectObjects(MouseEvent)
        else
            self:OnDragCamera(MouseEvent)
        end
    else
        self:OnDragCamera(MouseEvent)
    end

    self.StartWorldPos = WorldPosition
    self.MouseDownPos = screenPos
    
    -- print("[UGC]OnDragingMove, CameraPos=", CameraPos, ",moveOffset=", moveOffset)
end

function M:OnTransformSelectObjects(MouseEvent)    
    local Transfrom_Mode = _G.UGC.SelectInfo.Transfrom_Mode
    local OperatorType = _G.UGC.OperatorType
    if Transfrom_Mode == OperatorType.Tf_Move then
        self:OnMoveSelectObjects(MouseEvent)
    elseif Transfrom_Mode == OperatorType.Tf_Scale then
        self:ScaleSelectObjects(MouseEvent)
    elseif Transfrom_Mode == OperatorType.Tf_Rotaion then
        self:RotationSelectObjects(MouseEvent)
    end
end

-- 平移物体
function M:OnMoveSelectObjects(MouseEvent)
    local Dir, Right = self:CalcMoveInfo(MouseEvent)
    local NewObjPos = self:GetPickUpPos(MouseEvent)
    local MoveOffset = NewObjPos - self.StartPickUpPos

    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    for i = 1, #SelectObjects do
        local Obj = SelectObjects[i]
        local OldPos = Obj:K2_GetActorLocation()
        local NewPos = OldPos + MoveOffset
            
        local sweepHitResult = UE4.FHitResult()
        Obj:K2_SetActorLocation(NewPos, false, sweepHitResult, false)
    end    
    local CameraPos = self.StartCameraPos - Dir + Right
    -- local CameraPos = self.StartCameraPos + MoveOffset
    self:SetCameraPos(CameraPos)
    self.StartCameraPos = CameraPos

    NewObjPos = self:GetPickUpPos(MouseEvent)
    self.StartPickUpPos = NewObjPos

    _G.UGC.EventManager:SendUGCMessage("UGC.Event.TransformSelectObject")
end

-- 缩放物体
function M:ScaleSelectObjects(MouseEvent)  
    local NewObjPos = self:GetPickUpPos(MouseEvent)
    local MoveOffset = NewObjPos - self.StartPickUpPos

    local Dist = MoveOffset:Size()
    if Dist < 0.1 then
        return 
    end
    
    local OldBoundBoxInfo = self.FirstBoundBoxInfo
    local CurBoundBoxInfo = _G.UGC.OperatorMrg:GetFirstSelectBoundBoxInfo()

    local OldSize_X = OldBoundBoxInfo.BoxExtent.X
    local OldSize_Y = OldBoundBoxInfo.BoxExtent.Y
    local OldSize_Z = OldBoundBoxInfo.BoxExtent.Z

    local Scale = CurBoundBoxInfo.Scale

    local SelectAxis = _G.UGC.SelectInfo.SelectAxis
    if SelectAxis == _G.UGC.AxisType.Axis_X then
        Scale.X = (OldSize_X + MoveOffset.X) * OldBoundBoxInfo.Scale.X / OldSize_X
        Scale.X = self:AdjustScale(Scale.X)
    elseif SelectAxis == _G.UGC.AxisType.Axis_Y then
        Scale.Y = (OldSize_Y + MoveOffset.Y) * OldBoundBoxInfo.Scale.Y / OldSize_Y
        Scale.Y = self:AdjustScale(Scale.Y)
    elseif SelectAxis == _G.UGC.AxisType.Axis_Z then
        Scale.Z = (OldSize_Z + MoveOffset.Z) * OldBoundBoxInfo.Scale.Z / OldSize_Z
        Scale.Z = self:AdjustScale(Scale.Z)
    else
        Scale.X = (OldSize_X + MoveOffset.X) * OldBoundBoxInfo.Scale.X / OldSize_X
        Scale.Y = (OldSize_Y + MoveOffset.Y) * OldBoundBoxInfo.Scale.Y / OldSize_Y
        Scale.Z = (OldSize_Z + MoveOffset.Z) * OldBoundBoxInfo.Scale.Z / OldSize_Z
        Scale.X = self:AdjustScale(Scale.X)
        Scale.Y = self:AdjustScale(Scale.Y)
        Scale.Z = self:AdjustScale(Scale.Z)
    end
    
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    for i = 1, #SelectObjects do
        local Obj = SelectObjects[i]
        Obj:SetActorScale3D(Scale)
    end

    -- print("[UGC]ScaleSelectObjects, Dist=", Dist, ", Scale=", Scale, ",MoveOffset=", MoveOffset, ",CurBox:", CurBoundBoxInfo.BoxExtent)
    
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.TransformSelectObject")
end

function M:AdjustScale(NewScale)
    if NewScale < 0.001 then
        NewScale = 0.001
    end
    return NewScale
end

-- 旋转物体
function M:RotationSelectObjects(MouseEvent)
end

function M:OnDragCamera(MouseEvent)
    local CameraPos = self:CalcCameraMove(MouseEvent)
    self:SetCameraPos(CameraPos)
    self.StartCameraPos = CameraPos
end

-- 获得拾取的地面位置
function M:GetPickUpPos(MouseEvent)
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    local FirstSelectObj = SelectObjects[1]
    
    local LastObjPos = UE.FVector()
    if FirstSelectObj then
        LastObjPos = FirstSelectObj:K2_GetActorLocation()
    end
    local TerrainPlane = UE.FPlane(LastObjPos, UE.FVector(0, 0, 1))
    
    local WorldPosition = UE.FVector()
    local WorldDirection = UE.FVector()
    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    UE.UGameplayStatics.DeprojectScreenToWorld(localPlayerControler, ScreenPosition, WorldPosition, WorldDirection)
    local NewObjPos = TerrainPlane:IntersectLine(WorldPosition, WorldDirection)

    return NewObjPos
end

function M:GetCameraPos()
    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local palyerCameraManager = localPlayerControler.PlayerCameraManager
    local PlayPawn = localPlayerControler.Pawn
    return PlayPawn:K2_GetActorLocation()
end

function M:SetCameraPos(CameraPos)
    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local palyerCameraManager = localPlayerControler.PlayerCameraManager
    local PlayPawn = localPlayerControler.Pawn
    local sweepHitResult = UE4.FHitResult()
    PlayPawn:K2_SetActorLocation(CameraPos, false, sweepHitResult, false)
end

function M:CalcMoveInfo(MouseEvent, fScale)
    local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    local MoveX = ScreenPosition.X - self.MouseDownPos.X
    local MoveY = ScreenPosition.Y - self.MouseDownPos.Y

    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    -- local WorldPosition = self:ScreenToWorld(ScreenPosition)
    -- local WorldPosition = UE.FVector()
    -- local WorldDirection = UE.FVector()
    -- UE.UGameplayStatics.DeprojectScreenToWorld(localPlayerControler, ScreenPosition, WorldPosition, WorldDirection)
    local CameraRatation = localPlayerControler:K2_GetActorRotation()
    local CameraDir = CameraRatation:ToVector()
    local CameraRight = CameraRatation:GetRightVector()

    CameraDir.Z = 0
    CameraDir:Normalize()

    fScale = fScale or 0.5
    local Dir = CameraDir * (MoveY * fScale)
    local Right = CameraRight * (MoveX * fScale)

    return Dir, Right
end

function M:CalcCameraMove(MouseEvent)
    local Dir, Right = self:CalcMoveInfo(MouseEvent)
    local CameraPos = self.StartCameraPos + Dir - Right
    return CameraPos
end

function M:ScreenToWorld(ScreenPosition)
    -- local pickPos, hitResult = LineTraceUtil:GetPickupPosition(self, ScreenPosition)
    -- return pickPos

    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local WorldPosition = UE.FVector()
    local WorldDirection = UE.FVector()
    UE.UGameplayStatics.DeprojectScreenToWorld(localPlayerControler, ScreenPosition, WorldPosition, WorldDirection)
    local CameraPos = self:GetCameraPos()

    local Panel = UE.FPlane(CameraPos + WorldDirection * 200, WorldDirection)
    local ProjectPos = UE.FVector.PointPlaneProject(WorldPosition, Panel)
    -- print("[UGC]ScreenToWorld, CameraPos=", CameraPos, ",WorldPosition=", WorldPosition)

    return ProjectPos
end

function M:FindNearActor(screenPos)
    local playerControler = UE.UGameplayStatics.GetPlayerController(self:GetWorld(), 0)
    local objects = UGC.SceneObjects.Objects    
    local TotalCount = #objects

    -- 增加轮选功能
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    local StartIndex = 1
    if #SelectObjects == 1 then
        local SelectObj = SelectObjects[1]
        for i = 1, TotalCount do
            if objects[i] == SelectObj then
                StartIndex = i + 1
                if StartIndex > TotalCount then
                    StartIndex = 1
                end
                break
            end
        end
    end

    local findActor = self:InnerFindNearActor(playerControler, StartIndex, objects, screenPos)
    if findActor == nil then
        findActor = self:InnerFindNearActor(playerControler, 1, objects, screenPos)
    end
    return findActor
end

function M:InnerFindNearActor(playerControler, StartIndex, objects, screenPos)
    local sx = screenPos.X
    local sy = screenPos.Y
    local distMin = 1000000
    local findActor = nil
    for i = StartIndex, #objects do
        local obj = objects[i]
        local WorldPosition = obj:K2_GetActorLocation()
        local actorScreenPosition = UE.FVector2D()
        local bSuc = UE.UGameplayStatics.ProjectWorldToScreen(playerControler, WorldPosition, actorScreenPosition, false)
        if bSuc then
            -- print("[UGC][FindNearActor]actorScreenPosition:", actorScreenPosition, ",screenPos:", screenPos, ",name:", obj:GetName())
            local dx = sx - actorScreenPosition.X
            local dy = sy - actorScreenPosition.Y
            local dist = dx * dx + dy * dy
            if dist < distMin then
                distMin = dist
                findActor = obj
            end
        end
    end
    return findActor
end

function M:SetSelectEffect(selectActor)    
	local componentList = selectActor:K2_GetComponentsByClass(UE4.UMeshComponent)    
    print("[UGC]SetSelectEffect, component num:", #componentList)

    for _idx, component in pairs(componentList) do
        component:SetCustomDepthStencilValue(2)
        component:SetRenderCustomDepth(true)
    end
end

return M