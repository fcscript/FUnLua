-- W_UGC_ItemObjSwitch
require("UGC.Util.InitListView")
local M = {}

function M:Construct()
    self.CheckBox_List.OnCheckStateChanged:Add(self, self.OnCheckStateChanged)
    self.CheckBox_List:SetIsChecked(true)
    self.Name_List:SetText("组件列表")
    self.Switcher_List:SetActiveWidgetIndex(0)
end

function M:OnCheckStateChanged(bIsChecked)
    if bIsChecked then
        self.Name_List:SetText("组件列表")
        self.Switcher_List:SetActiveWidgetIndex(0)
    else
        self.Name_List:SetText("场景对象列表")
        self.Switcher_List:SetActiveWidgetIndex(1)
    end
end

return M