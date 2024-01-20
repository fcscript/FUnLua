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

    if selectActor then
        -- self:SetSelectEffect(selectActor)
        -- UE.USPLuaFunctionLibary.SetActorSelectEffect(selectActor)
        self:ShowAxisActor(selectActor)
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
    local CameraPos = self:CalcCameraMove(MouseEvent)
    
    self:SetCameraPos(CameraPos)

    self.StartCameraPos = CameraPos    
    self.StartWorldPos = WorldPosition
    self.MouseDownPos = screenPos
    
    -- print("[UGC]OnDragingMove, CameraPos=", CameraPos, ",moveOffset=", moveOffset)
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

function M:CalcCameraMove(MouseEvent)
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

    local Dir = CameraDir * (MoveY * 0.5)
    local Right = CameraRight * (MoveX * 0.5)
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

function M:CreateAxisObject(Location)    
    local world = self:GetWorld()
    local ActorClass = UE.AAxisActor    
    local objName = "AxisActor"
    local initRotation = UE.FRotator(0, 0, 0)
    local transform = UE.FTransform(initRotation:ToQuat(), Location)
    local moduleName = nil
    self.AxisActor = world:SpawnActor(ActorClass, transform, UE.ESpawnActorCollisionHandlingMethod.AlwaysSpawn, nil, nil, moduleName, nil, nil, objName)    
end

function M:ShowAxisActor(selectActor)
	local Location = selectActor:K2_GetActorLocation()
    if self.AxisActor == nil then
        self:CreateAxisObject(Location)
    end
    local SweepHitResult = UE.FHitResult()
    self.AxisActor:K2_GetRootComponent():K2_SetRelativeLocation(Location, false, SweepHitResult, false)
end

function M:FindNearActor(screenPos)
    local playerControler = UE.UGameplayStatics.GetPlayerController(self:GetWorld(), 0)
    local sx = screenPos.X
    local sy = screenPos.Y

    local UGC_SceneObjects = require("UGC.UI.UGC_SceneObjects")
    local objects = UGC_SceneObjects.Objects    
    local distMin = 1000000
    local findActor = nil
    for i = 1, #objects do
        local obj = objects[i]
        local WorldPosition = obj:K2_GetActorLocation()
        local actorScreenPosition = UE.FVector2D()
        local bSuc = UE.UGameplayStatics.ProjectWorldToScreen(playerControler, WorldPosition, actorScreenPosition, false)
        if bSuc then
            print("[UGC][FindNearActor]actorScreenPosition:", actorScreenPosition, ",screenPos:", screenPos, ",name:", obj:GetName())
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