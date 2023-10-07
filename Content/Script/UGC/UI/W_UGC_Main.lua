-- W_UGC_Main
local M = {}

function M:Construct()
    print("[UGC][Construct]W_UGC_Main")    
    local UGC_DragInfo = require("UGC.UI.UGC_DragInfo")
    UGC_DragInfo.MainRoot = self.MainRoot
end

function M:Destruct()    
    print("[UGC][Destruct]W_UGC_Main")
end

function M:OnInitialized()
    print("[UGC][OnInitialized]W_UGC_Main")
end

return M