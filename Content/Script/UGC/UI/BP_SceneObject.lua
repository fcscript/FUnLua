-- BP_TemplateItem
local M = {}

function M:Construct()
    print("[UGC][Construct]BP_TemplateItem")
end

function M:Destruct()    
    print("[UGC][Destruct]BP_TemplateItem")
end

function M:OnInitialized()
    print("[UGC][OnInitialized]BP_TemplateItem")
end

return M