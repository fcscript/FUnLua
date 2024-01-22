-- W_UGC_ObjList
local M = {}

function M:Construct()
    print("[UGC][Construct]W_UGC_ObjList")
    _G.UGC.EventManager:ListenUGCMessage("UGC.Event.AddObject", self, M.OnAddObject)
    _G.UGC.EventManager:ListenUGCMessage("UGC.Event.DeleteSelects", self, M.OnDeleteSelects)
    _G.UGC.EventManager:ListenUGCMessage("UGC.Event.SelectObjectFromScene", self, M.OnSelectObjectFromScene)
end

function M:Destruct()    
    print("[UGC][Destruct]W_UGC_ObjList")
    _G.UGC.EventManager:UnListenUGCMessageByPtr("UGC.Event.AddObject", self)
    _G.UGC.EventManager:UnListenUGCMessageByPtr("UGC.Event.DeleteSelects", self)
    _G.UGC.EventManager:UnListenUGCMessageByPtr("UGC.Event.SelectObjectFromScene", self)
    _G.GlbClearListViewCache(self.ListView_Objects)
end

function M:OnInitialized()
    print("[UGC][OnInitialized]W_UGC_ObjList")
end

function M:OnAddObject(object)
    print("[UGC][OnAddObject]object=", object)
    self:RefreshList()    
end

function M:OnDeleteSelects(Objects)
    print("[UGC][OnAddObject]Objects Count=", #Objects)
    self:RefreshList()
end

function M:OnSelectObjectFromScene(SelectObject)
    -- local ObjID = UnLua.GetObjRefID(self.ListView_Objects)
    -- print("[UGC][OnSelectObjectFromScene]ListView_Objects=", self.ListView_Objects, ",ObjID=", ObjID)
    _G.GlbViewAllListItem(self.ListView_Objects, function(ListView, Item)
        if Item.Object == SelectObject then
            _G.GlbSelectListViewItem(self.ListView_Objects, Item)
        end
    end)
end

function M:RefreshList()

    local CustomData = 
    {
        RootPanel = self,
        ListView = self.ListView_Template,
    }

    local itemBPClassPath = "/Game/UGC/UI/BP_UGC_ObjectInfo.BP_UGC_ObjectInfo_C"
    local itemClass = UE.UClass.Load(itemBPClassPath)

    local Objects = _G.UGC.SceneObjects.Objects
    local entryBindScript = "UGC.UI.BP_SceneObject"
    _G.GlbRefreshListView(CustomData, self.ListView_Objects, itemClass, entryBindScript, Objects, function(item, sourceInfo, index)
        item.BPPath = ""
        item.ID = 0
        item.Name = sourceInfo:GetName()
        item.Object = sourceInfo
    end)
end

return M