-- W_UGC_ObjList
local M = {}

function M:Construct()
    print("[UGC][Construct]W_UGC_ObjList")
    local UGC_EventManager = require("UGC.Event.UGC_EventManager")
    UGC_EventManager:ListenUGCMessage("UGC.Event.AddObject", self, M.OnAddObject)
end

function M:Destruct()    
    print("[UGC][Destruct]W_UGC_ObjList")
    local UGC_EventManager = require("UGC.Event.UGC_EventManager")
    UGC_EventManager:UnListenUGCMessageByPtr("UGC.Event.AddObject", self)
    _G.GlbClearListViewCache(self.ListView_Objects)
end

function M:OnInitialized()
    print("[UGC][OnInitialized]W_UGC_ObjList")
end

function M:OnAddObject(object)
    print("[UGC][OnAddObject]object=", object)
    self:RefreshList()    
end

function M:RefreshList()

    local CustomData = 
    {
        RootPanel = self,
        ListView = self.ListView_Template,
    }
    local UGC_SceneObjects = require("UGC.UI.UGC_SceneObjects")

    local itemBPClassPath = "/Game/UGC/UI/BP_UGC_ObjectInfo.BP_UGC_ObjectInfo_C"
    local itemClass = UE.UClass.Load(itemBPClassPath)

    local entryBindScript = "UGC.UI.BP_SceneObject"
    _G.GlbRefreshListView(CustomData, self.ListView_Objects, itemClass, entryBindScript, UGC_SceneObjects.Objects, function(item, sourceInfo, index)
        item.BPPath = ""
        item.ID = 0
        item.Name = sourceInfo:GetName()
    end)
end

return M