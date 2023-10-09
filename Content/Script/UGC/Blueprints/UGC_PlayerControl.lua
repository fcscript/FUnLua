-- UGC_PlayerControl
local M = {}

function M:LeftMouseButton_Pressed(key)
    -- print("[UGC][LeftMouseButton_Pressed]key=", key)
    -- local playerControler = UE.UGameplayStatics.GetPlayerController(self:GetWorld(), 0)

    local traceChannel = UE.ECollisionChannel.WorldStatic
    local hitResult = UE.FHitResult()
    local bSuc = self:GetHitResultUnderCursor(traceChannel, true, hitResult)
    local HitObjectHandle = hitResult.HitObjectHandle
    local hitActor = HitObjectHandle.Actor:Get()
    -- local parentActor = self:GetRootActor(hitResult.Component:Get())
    -- UnLua.DebugCheck(hitActor)
    -- print("[UGC][GetHitResultUnderCursor]traceChannel=", traceChannel, "bSuc=", bSuc, ",hitResult=", hitResult,  ",hitActor=", hitActor)
end

function M:GetRootActor(actorComponent)
    local rootComponent = actorComponent
    while actorComponent ~= nil do
        rootComponent = actorComponent
        local actor = rootComponent:GetOuter()
        print("[UGC]actor Name:", actor:GetName())
        actorComponent = actorComponent:GetAttachParent()
    end
    return rootComponent:GetOuter()
end

function M:LeftMouseButton_Released(key)    
    -- print("[UGC][LeftMouseButton_Released]key=", key)
end

return M