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
    print("[UGC]RefreshListEntry, bSelect=", bSelect, ",Name:", Item.Name)
    self.CustomInfo = CustomInfo
    self.Item = Item
    self.ID = Item.ID
    self.LB_ItemName:SetText(Item.Name)
end

function M:SelectListEntry(bSelect)
    print("[UGC]SelectListEntry, bSelect=", bSelect, ",ID=", self.ID)
end

return M