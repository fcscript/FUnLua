-- LineTraceUtil
local M = {}
M.__index = M

-- 获得屏幕射线拾取的位置
function M:GetPickupPosition(worldContex, screenPos)    
    local palyerControl = UE.UGameplayStatics.GetPlayerController(worldContex, 0)
    local WorldLocation = UE.FVector()
    local WorldDirection = UE.FVector()
    local bSuc = UE.UGameplayStatics.DeprojectScreenToWorld(palyerControl, screenPos, WorldLocation, WorldDirection)
    -- local bSuc = palyerControl:DeprojectMousePositionToWorld(WorldLocation, WorldDirection)
    if not bSuc then
        local playerCharacter = UE.UGameplayStatics.GetPlayerCharacter(worldContex, 0)
        local pickPos = self:GetPlayerFacePostion(playerCharacter, 100)
        -- print("[UGC]GetPickupPosition, failed Project camear, pickPos=", pickPos)
        return pickPos
    end

    local endPos = WorldLocation + WorldDirection * 100000
    local traceChannel = 0 -- UE.ECollisionChannel.Visibility
    local ActorsToIgnore = UE.TArray(UE.AActor)
    local hitResult = UE.FHitResult()
    local TraceColor = UE.FLinearColor(1, 0, 0, 1)
    local TraceHitColor = UE.FLinearColor(0, 1, 0, 1)
    local bHit = UE.UKismetSystemLibrary.LineTraceSingle(worldContex,WorldLocation,endPos,traceChannel,false,ActorsToIgnore, 1, hitResult, true,TraceColor,TraceHitColor,5)
    
    if bHit then
        -- print("[UGC]GetPickupPosition, bHit is true, hitPos=", hitResult.Location)
        return hitResult.Location
    else
        -- print("[UGC]GetPickupPosition, bHit is false, near pos=", WorldLocation + WorldDirection * 10000)
        return WorldLocation + WorldDirection * 10000
    end
end

function M:GetPlayerFacePostion(playerCharacter, dist)    
    local playerPos = playerCharacter:K2_GetActorLocation()
    local playerRotation = playerCharacter:K2_GetActorRotation()
    local playerDir = playerRotation:ToVector()
    playerDir.Pitch = 0
    playerDir.Roll = 0
    local facePos = playerPos + playerDir * dist
    return facePos
end

return M