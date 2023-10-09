-- W_UGC_Backpanel
local M = {}

function M:OnMouseButtonDown(MyGeometry, MouseEvent)    
    local vecLU = UE.FVector2D(0, 0)
    local selfScreenPos = UE4.USlateBlueprintLibrary.LocalToAbsolute(MyGeometry, vecLU)

    local screenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    local panelPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(MyGeometry, screenPos)  -- 转换成本地坐标
    -- print("[UGC]panelPos:", panelPos, ",screenPos:", screenPos, ",selfScreenPos:", selfScreenPos)

    -- 真实的屏幕坐标 = 当前屏幕坐标 - 左上角的屏幕坐标
    local realScreenPos = UE.FVector2D(screenPos.X - selfScreenPos.X, screenPos.Y - selfScreenPos.Y)
    local selectActor = self:FindNearActor(realScreenPos)
    
    -- local LineTraceUtil = require("UGC.Util.LineTraceUtil")
    -- local pickPos, hitResult = LineTraceUtil:GetPickupPosition(self, panelPos)
    -- if hitResult then
    --     local HitObjectHandle = hitResult.HitObjectHandle
    --     local hitActor = HitObjectHandle.Actor:Get()
    --     print("[UGC]Find HitResult, hitActor:", hitActor)
    -- end

	return UE.UWidgetBlueprintLibrary.Unhandled()
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
            print("[UGC][FindNearActor]actorScreenPosition:", actorScreenPosition, ",screenPos:", screenPos)
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

return M