-- W_UGC_ObjEntry
local M = {}
M.__index = M

function M:Construct()
end

function M:Destruct()    
end

function M:OnInitialized()
end

function M:RefreshListEntry(CustomInfo, Item, bSelect)
    print("[UGC]RefreshListEntry, bSelect=", bSelect, ",Name:", Item.Name, ",Object:", Item.Object)
    self.CustomInfo = CustomInfo
    self.Item = Item
    self.ID = Item.ID
    self.Object = Item.Object
    self.LB_ItemName:SetText(Item.Name)
    self:RefreshSelectFlag(bSelect)
end

function M:SelectListEntry(bSelect)
    print("[UGC]SelectListEntry, bSelect=", bSelect, ",ID=", self.ID, ",Item=", self.Item, ",Object=", self.Object)
    self:RefreshSelectFlag(bSelect)
    if bSelect then
        _G.UGC.SelectInfo.SelectObjects = { self.Object }
        _G.UGC.EventManager:SendUGCMessage("UGC.Event.SelectObject", self.Object)
    end    
end

function M:RefreshSelectFlag(bSelect)
    if bSelect then
        self.Image_Item:SetColorAndOpacity(UE.FLinearColor(0, 0.267361, 1, 1))
    else
        self.Image_Item:SetColorAndOpacity(UE.FLinearColor(1, 1, 1, 1))
    end
end

function M:OnDoubleClicked()
    UnLua.DebugBreak(true)
    print("[UGC]OnDoubleClicked, Object=", self.Object)
    -- 切换摄像机, 将摄像机移动这里
    local ObjPos = self.Object:K2_GetActorLocation()
    -- self:SetCameraPos(ObjPos)

    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local WorldPosition = ObjPos
    local ScreenPosition = UE.FVector2D()
    local bSuc = UE.USPLuaFunctionLibary.ProjectWorldToScreen(localPlayerControler, WorldPosition, ScreenPosition, false)
    
    local StartCameraPos = localPlayerControler:K2_GetActorLocation()
    local CameraScreenPosition = UE.FVector2D()
    UE.USPLuaFunctionLibary.ProjectWorldToScreen(localPlayerControler, StartCameraPos, CameraScreenPosition, false)
    local Cx = UE.USPLuaFunctionLibary.GetClientViewWidth() * 0.5
    local Cy = UE.USPLuaFunctionLibary.GetClientViewHeight() * 0.5

    local MoveX = Cx - ScreenPosition.X
    local MoveY = Cy - ScreenPosition.Y

    local CameraRatation = localPlayerControler:K2_GetActorRotation()
    local CameraDir = CameraRatation:ToVector()
    local CameraRight = CameraRatation:GetRightVector()

    CameraDir.Z = 0
    CameraDir:Normalize()

    local Dir = CameraDir * (MoveY * 0.5)
    local Right = CameraRight * (MoveX * 0.5)
    local CameraPos = StartCameraPos + Dir - Right

    self:SetCameraPos(CameraPos)

    print("[UGC]OnDoubleClicked, Object=", self.Object, ",ScreenPosition=", ScreenPosition, ",bSuc=", bSuc, ",CameraPos=", CameraPos, ",MoveX=", MoveX, ",MoveY=", MoveY)
end

function M:SetCameraPos(CameraPos)
    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(self, 0)
    local palyerCameraManager = localPlayerControler.PlayerCameraManager
    local PlayPawn = localPlayerControler.Pawn
    local sweepHitResult = UE4.FHitResult()
    PlayPawn:K2_SetActorLocation(CameraPos, false, sweepHitResult, false)
end

return M