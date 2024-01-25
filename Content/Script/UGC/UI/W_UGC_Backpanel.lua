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
    elseif not bSelectAxis then
        _G.UGC.OperatorMrg:SelectObjectFromScene(nil)
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
    self.StartMouseDownPos = UE.FVector2D(screenPos.X, screenPos.Y)
    self.StartWorldPos = self:ScreenToWorld(screenPos)
    self.StartCameraPos = self:GetCameraPos()
    self.StartTerrainPickPos = self:GetTerranPickPos(MouseEvent)
    self.StartFacePickPos = self:GetFacePickPos(MouseEvent)
    self.FirstBoundBoxInfo = _G.UGC.OperatorMrg:GetFirstSelectBoundBoxInfo()
   
    -- print("[UGC]OnMouseButtonDown, bMouseDown=", self.bMouseDown, ",StartCameraPos=", self.StartCameraPos)
    if self.bMouseDown then
        local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
        localPlayerControler.bShowMouseCursor = 1
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
        local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
        localPlayerControler.bShowMouseCursor = 0
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
    local NewTerranPos = self:GetTerranPickPos(MouseEvent)
    local NewFacePos = self:GetFacePickPos(MouseEvent)
    local MoveOffset = NewTerranPos - self.StartTerrainPickPos
    local FaceMove = NewFacePos - self.StartFacePickPos    

    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    local SelectAxis = _G.UGC.SelectInfo.SelectAxis
    if SelectAxis == _G.UGC.AxisType.Axis_X then
        for i = 1, #SelectObjects do
            local Obj = SelectObjects[i]
            local NewPos = Obj:K2_GetActorLocation()
            NewPos.X = NewPos.X + MoveOffset.X
                
            local sweepHitResult = UE4.FHitResult()
            Obj:K2_SetActorLocation(NewPos, false, sweepHitResult, false)
        end
    elseif SelectAxis == _G.UGC.AxisType.Axis_Y then
        for i = 1, #SelectObjects do
            local Obj = SelectObjects[i]
            local NewPos = Obj:K2_GetActorLocation()
            NewPos.Y = NewPos.Y + MoveOffset.Y
                
            local sweepHitResult = UE4.FHitResult()
            Obj:K2_SetActorLocation(NewPos, false, sweepHitResult, false)
        end
    elseif SelectAxis == _G.UGC.AxisType.Axis_Z then
        for i = 1, #SelectObjects do
            local Obj = SelectObjects[i]
            local NewPos = Obj:K2_GetActorLocation()
            NewPos.Z = NewPos.Z + FaceMove.Z
                
            local sweepHitResult = UE4.FHitResult()
            Obj:K2_SetActorLocation(NewPos, false, sweepHitResult, false)
        end
    else
        for i = 1, #SelectObjects do
            local Obj = SelectObjects[i]
            local OldPos = Obj:K2_GetActorLocation()
            local NewPos = OldPos + MoveOffset
                
            local sweepHitResult = UE4.FHitResult()
            Obj:K2_SetActorLocation(NewPos, false, sweepHitResult, false)
        end    
    end

    local CameraPos = self.StartCameraPos - Dir + Right
    -- local CameraPos = self.StartCameraPos + MoveOffset
    self:SetCameraPos(CameraPos)
    self.StartCameraPos = CameraPos

    self.StartTerrainPickPos = self:GetTerranPickPos(MouseEvent)
    self.StartFacePickPos = self:GetFacePickPos(MouseEvent)

    _G.UGC.EventManager:SendUGCMessage("UGC.Event.TransformSelectObject")
end

-- 缩放物体
function M:ScaleSelectObjects(MouseEvent)  
    local NewTerranPos = self:GetTerranPickPos(MouseEvent)
    local NewFacePos = self:GetFacePickPos(MouseEvent)
    local MoveOffset = NewTerranPos - self.StartTerrainPickPos
    local FaceMove = NewFacePos - self.StartFacePickPos    

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
        Scale.Z = (OldSize_Z + FaceMove.Z) * OldBoundBoxInfo.Scale.Z / OldSize_Z
        Scale.Z = self:AdjustScale(Scale.Z)
    else
        Scale.X = (OldSize_X + MoveOffset.X) * OldBoundBoxInfo.Scale.X / OldSize_X
        Scale.Y = (OldSize_Y + MoveOffset.Y) * OldBoundBoxInfo.Scale.Y / OldSize_Y
        Scale.Z = (OldSize_Z + FaceMove.Z) * OldBoundBoxInfo.Scale.Z / OldSize_Z
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
    -- print("[UGC]FaceMove:", FaceMove, ",FaceDist=", FaceDist, ",SelectAxis=", SelectAxis)
    
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
function M:GetTerranPickPos(MouseEvent)
    local Origin = _G.UGC.OperatorMrg:GetFirstSelectActorPosition()
    local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    return _G.UGC.OperatorMrg:GetTerranPickPos(Origin, ScreenPosition)
end

-- 获得正面拾取的位置
function M:GetFacePickPos(MouseEvent)    
    local Origin = _G.UGC.OperatorMrg:GetFirstSelectActorPosition()
    local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    return _G.UGC.OperatorMrg:GetCameraPickPos(Origin, ScreenPosition)
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

function M:Calc3DMove(MouseEvent, StartDownPos, fScale)
    local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    local MoveX = ScreenPosition.X - StartDownPos.X
    local MoveY = ScreenPosition.Y - StartDownPos.Y

    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local CameraRatation = localPlayerControler:K2_GetActorRotation()
    local CameraDir = CameraRatation:ToVector()
    local CameraRight = CameraRatation:GetRightVector()

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
                if self:IsCanSelect(playerControler, obj, screenPos ) then
                    distMin = dist
                    findActor = obj
                end
            end
        end
    end
    return findActor
end

function M:IsCanSelect(playerControler, Actor, screenPos)
    local Origin = UE.FVector()
    local BoxExtent = UE.FVector()
    Actor:GetActorBounds(true, Origin, BoxExtent, true)

    local S0 = UE.FVector2D()
    UE.UUGCFunctionLibary.ProjectWorldToScreen(playerControler, Origin, S0, false)
    local Left = S0.X
    local Right = S0.X
    local Top = S0.Y
    local Bottom = S0.Y

    ----      V7 ----- V8
    ----     /        / | 
    ---     /        /  |
    ----   V5 ----- V6  |
    ----   |        |   |
    ----   |  V3    | V4
    ----   | /      | /
    ----   V1 ----- V2 

    local V1 = Origin - BoxExtent
    local V8 = Origin + BoxExtent
    
    local Points = { V1, V8 }
    local S1 = UE.FVector2D()
    for i = 1, #Points do
        UE.UUGCFunctionLibary.ProjectWorldToScreen(playerControler, Points[i], S1, false)
        Left = math.min(Left, S1.X)
        Right = math.max(Right, S1.X)
        Top = math.min(Top, S1.Y)
        Bottom = math.max(Bottom, S1.Y)
    end
    local X = screenPos.X
    local Y = screenPos.Y
    return X > Left and X < Right and Y > Top and Y < Bottom
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