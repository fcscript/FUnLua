-- W_UGC_Tools
local M = {}

function M:Construct()
    print("[UGC][Construct]W_UGC_Tools")    
    self:InitButtonList()
end

function M:Destruct()    
    print("[UGC][Destruct]W_UGC_Tools")
end

function M:OnInitialized()
    print("[UGC][OnInitialized]W_UGC_Tools")
end

function M:InitButtonList()
    local buttonBpClassPath = "/Game/UGC/UI/W_UGC_Button.W_UGC_Button_C"
    local buttonBpClass = UE.UClass.Load(buttonBpClassPath)
    self.ButtonBpClass = buttonBpClass
    self.HB_FuncButtonList:ClearChildren()
    self:AddButton("Test", M.OnTest)
    
    self:AddButton("移动", M.OnMoveMode)
    self:AddButton("旋转", M.OnRotationMode)
    self:AddButton("缩放", M.OnScaleMode)
    self:AddButton("删除", M.OnDeleteSelect)
    self:AddButton("删除全部", M.OnDeleteAll)
end

function M:AddButton(title, cb)
    local Widget = UE.UWidgetBlueprintLibrary.Create(self, self.ButtonBpClass)
    self.HB_FuncButtonList:AddChild(Widget)
    Widget.Button_Func.OnClicked:Add(self, cb)
    Widget.LB_ButtonName:SetText(title)
end

function M:OnTest()    
    local initRotation = UE.FRotator(0, 0, 0)
    local quat = initRotation:ToQuat()
    local hitResult = UE.FHitResult()
    hitResult.Location.X = 100
    hitResult.Location.Y = 105
    hitResult.Location.Z = 190
    local hitPos = hitResult.Location
    local transform = UE.FTransform(quat, hitPos)
    print("[UGC]hitResult.Location:", hitResult.Location, ",transform:", transform)
end

function M:OnMoveMode()
end

function M:OnRotationMode()
end

function M:OnScaleMode()
end

function M:OnDeleteSelect()    
end

function M:OnDeleteAll()    
end

return M