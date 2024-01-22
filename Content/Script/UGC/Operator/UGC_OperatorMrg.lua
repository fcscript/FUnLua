-- UGC_OperatorMrg
local M = {}
M.__index = M

function M:Init(GameInstance)
    self.GameInstance = GameInstance
    local EventManager = _G.UGC.EventManager
    EventManager:ListenUGCMessage("UGC.Event.AddObject", self, M.OnAddObject)
    EventManager:ListenUGCMessage("UGC.Event.DeleteSelects", self, M.DeleteSelects)
    EventManager:ListenUGCMessage("UGC.Event.SelectObject", self, M.SelectObject)
end

function M:OnAddObject(Object)
end

function M:DeleteSelects(SelectObjects)
    self:HideAxisActor()
end

function M:SelectObject(SelectObj)
    self:ShowAxisActor(SelectObj)
end

function M:SelectObjectFromScene(SelectObj)
    self:ShowAxisActor(SelectObj)
    _G.UGC.SelectInfo.SelectObjects = { SelectObj }
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.SelectObjectFromScene", SelectObj)
end

function M:GetWorld()
    return self.GameInstance:GetWorld()
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
    self.AxisActor:SetActorHiddenInGame(false)
    local SweepHitResult = UE.FHitResult()
    self.AxisActor:K2_GetRootComponent():K2_SetRelativeLocation(Location, false, SweepHitResult, false)
end

function M:HideAxisActor()
    if self.AxisActor then
        self.AxisActor:SetActorHiddenInGame(true)
    end    
end

return M