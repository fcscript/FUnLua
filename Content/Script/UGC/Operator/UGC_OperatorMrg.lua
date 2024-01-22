-- UGC_OperatorMrg
local M = {}
M.__index = M

function M:Init(GameInstance)
    self.GameInstance = GameInstance
    local EventManager = _G.UGC.EventManager
    EventManager:ListenUGCMessage("UGC.Event.AddObject", self, M.OnAddObject)
    EventManager:ListenUGCMessage("UGC.Event.DeleteSelects", self, M.DeleteSelects)
    EventManager:ListenUGCMessage("UGC.Event.SelectObject", self, M.SelectObject)
    EventManager:ListenUGCMessage("UGC.Event.TransformSelectObject", self, M.TransformSelectObject)
    EventManager:ListenUGCMessage("UGC.Event.ChangeTransfromMode", self, M.ChangeTransfromMode)
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
    self:ChangeTransfromMode()
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.SelectObjectFromScene", SelectObj)
end

function M:TransformSelectObject()
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    local Location = UE.FVector()
    for i = 1, #SelectObjects do
        local SelectObject = SelectObjects[i]
        Location = Location + SelectObject:K2_GetActorLocation()        
    end
    local Count = #SelectObjects
    if Count > 0 then
        local Scale = 1.0 / Count
        Location = Location * Scale
    end
    if self.AxisActor then
        local SweepHitResult = UE.FHitResult()
        self.AxisActor:K2_GetRootComponent():K2_SetRelativeLocation(Location, false, SweepHitResult, false)
    end
end

function M:ChangeTransfromMode()
    if self.AxisActor == nil then
        return 
    end
    local Transfrom_Mode = _G.UGC.SelectInfo.Transfrom_Mode
    local OperatorType = _G.UGC.OperatorType

    -- 平移
    if Transfrom_Mode == OperatorType.Tf_Move then
        -- 隐藏旋转
        self.AxisActor.RotationComponent.bRenderVisibility = false
        self.AxisActor.BoxComponent.bRenderVisibility = false
        
        self.AxisActor.TransfromComponentX.bRenderVisibility = true
        self.AxisActor.TransfromComponentY.bRenderVisibility = true
        self.AxisActor.TransfromComponentZ.bRenderVisibility = true

        self.AxisActor.TransfromComponentX.Length = 200
        self.AxisActor.TransfromComponentY.Length = 200
        self.AxisActor.TransfromComponentZ.Length = 200
    elseif Transfrom_Mode == OperatorType.Tf_Rotaion then
        -- 旋转
        self.AxisActor.RotationComponent.bRenderVisibility = true
        self.AxisActor.BoxComponent.bRenderVisibility = false

        self.AxisActor.TransfromComponentX.bRenderVisibility = false
        self.AxisActor.TransfromComponentY.bRenderVisibility = false
        self.AxisActor.TransfromComponentZ.bRenderVisibility = false
    elseif Transfrom_Mode == OperatorType.Tf_Scale then
        -- 缩放
        self.AxisActor.RotationComponent.bRenderVisibility = false
        self.AxisActor.BoxComponent.bRenderVisibility = false

        self.AxisActor.TransfromComponentX.bRenderVisibility = true
        self.AxisActor.TransfromComponentY.bRenderVisibility = true
        self.AxisActor.TransfromComponentZ.bRenderVisibility = true
    else        
        self.AxisActor.RotationComponent.bRenderVisibility = false
        self.AxisActor.BoxComponent.bRenderVisibility = true

        self.AxisActor.TransfromComponentX.bRenderVisibility = false
        self.AxisActor.TransfromComponentY.bRenderVisibility = false
        self.AxisActor.TransfromComponentZ.bRenderVisibility = false
    end
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