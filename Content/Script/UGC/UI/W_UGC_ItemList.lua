-- W_UGC_ItemList    
require("UGC.Util.InitListView")
local M = {}

function M:Construct()
    print("[UGC][Construct]W_UGC_ItemList")
    local itemBPClassPath = "/Game/UGC/UI/BP_UGC_ItemTemplateInfo.BP_UGC_ItemTemplateInfo_C"
    local itemClass = UClass.Load(itemBPClassPath)

    local objCfgs = 
    {
        {
            Name = "栅栏1",
            ID = 1,
            BpPath = "/Game/UGC/ItemTemplate/ZhaLan1.ZhaLan1",
        },
        {
            Name = "栅栏2",
            ID = 2,
            BpPath = "/Game/UGC/ItemTemplate/ZhaLan1.ZhaLan1",
        },
    }
    for i = 1, 10 do
        local ID = #objCfgs + 1
        local objInfo = 
        {
            Name = "栅栏" .. tostring(ID),
            ID = ID,
            BpPath = "/Game/UGC/ItemTemplate/ZhaLan1.ZhaLan1_C",
        }
        objCfgs[#objCfgs+1] = objInfo
    end

    local CustomData = 
    {
        RootPanel = self,
        ListView = self.ListView_Template,
    }
    local entryBindScript = "UGC.UI.BP_TemplateItem"
    _G.GlbRefreshListView(CustomData, self.ListView_Template, itemClass, entryBindScript, objCfgs, function(item, sourceInfo, index)
        item.BPPath = sourceInfo.BPPath
        item.ID = sourceInfo.ID
        item.Name = sourceInfo.Name
    end)
end

function M:Destruct()    
    print("[UGC][Destruct]W_UGC_ItemList")
    _G.GlbClearListViewCache(self.ListView_Template)
end

function M:OnInitialized()
    print("[UGC][OnInitialized]W_UGC_ItemList")
end

return M