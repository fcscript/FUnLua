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
    self:AddButton("取消选中", M.OnCancelMode)
    
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

function M:OnCancelMode()
    _G.UGC.SelectInfo.Transfrom_Mode = _G.UGC.OperatorType.Tf_None
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.ChangeTransfromMode", _G.UGC.SelectInfo.Transfrom_Mode)
end

function M:OnMoveMode()
    _G.UGC.SelectInfo.Transfrom_Mode = _G.UGC.OperatorType.Tf_Move
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.ChangeTransfromMode", _G.UGC.SelectInfo.Transfrom_Mode)
end

function M:OnRotationMode()
    _G.UGC.SelectInfo.Transfrom_Mode = _G.UGC.OperatorType.Tf_Rotaion
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.ChangeTransfromMode", _G.UGC.SelectInfo.Transfrom_Mode)
end

function M:OnScaleMode()
    _G.UGC.SelectInfo.Transfrom_Mode = _G.UGC.OperatorType.Tf_Scale
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.ChangeTransfromMode", _G.UGC.SelectInfo.Transfrom_Mode)
end

function M:OnDeleteSelect()
    local SelectObjcts = _G.UGC.SelectInfo.SelectObjects
    local AllObjects = _G.UGC.SceneObjects.Objects or {}
    local OldCount = #AllObjects
    _G.UGC.SelectInfo.SelectObjects = {}
    for _idx, Object in pairs(SelectObjcts) do
        Object:K2_DestroyActor()
        for i = #AllObjects, 1, -1 do
            if AllObjects[i] == Object then
                table.remove(AllObjects, i)
                break
            end
        end
    end
    _G.UGC.SceneObjects.Objects = AllObjects
    print("[UGC]DeleteSelect, OldCount=", OldCount, ",CurCount=", #AllObjects)
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.DeleteSelects", SelectObjcts)
end

function M:OnDeleteAll()
    local AllObjects = _G.UGC.SceneObjects.Objects or {}
    _G.UGC.SceneObjects.Objects = {}
    for _idx, Object in pairs(AllObjects) do
        Object:K2_DestroyActor()
    end
    _G.UGC.EventManager:SendUGCMessage("UGC.Event.DeleteSelects", AllObjects)
end

return M