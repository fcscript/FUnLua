---@type BP_HeadUIActor
local M = UnLua.Class()

function M:ReceiveBeginPlay()
    self.Overridden:ReceiveBeginPlay(self)    
    -- self.CheckBillboardHandle = UE.UKismetSystemLibrary.K2_SetTimerDelegate({self, M.CheckBillboard}, 0.01, true)
    UnLua.Log("[HeadActor]ReceiveBeginPlay, Name:", self:GetName()) 
end

function M:ReceiveTick(DeltaSeconds)
    self:CheckBillboard()
    UnLua.Log("[HeadActor]ReceiveTick, Name:", self:GetName()) 
end

function M:CheckBillboard()
	local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local palyerCameraManager = localPlayerControler.PlayerCameraManager
    local pov = palyerCameraManager.ViewTarget.POV
    -- self:PrintPOV(pov)  
    local initRotation = UE.FRotator(0, 180, 0)
    local caramaDir = pov.Rotation:ToVector()
    local billlboardDir = initRotation:RotateVector(caramaDir)
    local billboardRotation = billlboardDir:ToRotator()
    billboardRotation.Pitch = 0
    billboardRotation.Roll = 0
    self:K2_SetActorRotation(billboardRotation, false)
    -- UnLua.Log("[HeadActor]Camera Rotation:", pov.Rotation, ",billboardRotation:", billboardRotation)
    -- self.CheckBillboardHandle = UE.UKismetSystemLibrary.K2_SetTimerForNextTickDelegate({self, M.CheckBillboard})
end
function M:PrintPOV(pov)
    UnLua.Log("[HeadActor]Location:", pov.Location, ",Rotation:", pov.Rotation, ",FOV=",pov.FOV, ",DesiredFOV:", pov.DesiredFOV, ",OrthoWidth:", pov.OrthoWidth, ",AspectRatio:", pov.AspectRatio, ",bConstrainAspectRatio:", pov.bConstrainAspectRatio)
end

return M