---@type InitListView

local ListMapInfos = {}

local function RequireListViewCache(ListView)
    local ListInfo = ListMapInfos[ListView]
    if ListInfo == nil then
        ListInfo = 
        {
            Item2Widget = {},
            Widget2Item = {},
            SelectItem = nil,
            CustomInfo = nil,
        }
        ListMapInfos[ListView] = ListInfo
    end
    return ListInfo
end

local function MapListEntry(ListView, Item, Widget)
    local ListInfo = RequireListViewCache(ListView)
    ListInfo.Item2Widget[Item] = Widget
    ListInfo.Widget2Item[Widget] = Item
end

local function FindListEntry(ListView, Item)
    local ListInfo = ListMapInfos[ListView]
    if ListInfo then
        local entry = ListInfo.Item2Widget[Item]
        return entry
    end
    return nil
end

-- 通知节点选中的事件
local function  NotifyItemSelect(ListInfo, Item, bSelect)
    if ListInfo and Item then
        local entry = ListInfo.Item2Widget[Item]
        if entry and entry.SelectListEntry then
            entry:SelectListEntry(bSelect)
        end
    end
end

-- 通知列列的刷新
local function NotifyInitializedWidget(ListView, Item, Widget)
    -- 单行选中
    local selectItem = ListView:BP_GetSelectedItem()
    local bSelect = selectItem == Item
    if Widget.RefreshListEntry then
        local ListInfo = ListMapInfos[ListView] or {}
        Widget:RefreshListEntry(ListInfo.CustomInfo, Item, bSelect)
    end
end

-- 列表的初始化事件
local function  OnEntryInitialized(ListView, Item, Widget)
    MapListEntry(ListView, Item, Widget)
    NotifyInitializedWidget(ListView, Item, Widget)
end

-- 列表的点选事件
local function  OnItemClicked(ListView, Item)
    local entry = FindListEntry(ListView, Item)
    if entry then
    end
end

-- 列表的双击事件
local function  OnItemDoubleClicked(ListView, Item)
end

-- 列表的鼠标敏感事件(鼠标滑入)
local function  OnItemIsHoveredChanged(ListView, Item, bIsSelected)
end

-- 列表的选中事件
local function  OnItemSelectionChanged(ListView, Item, bIsSelected)
    local ListInfo = RequireListViewCache(ListView)
    local oldSelectItem = ListInfo.SelectItem
    ListInfo.SelectItem = Item
    NotifyItemSelect(ListInfo, oldSelectItem, false)
    NotifyItemSelect(ListInfo, Item, true)
end

-- 列表滑动后刷新事件
local function  OnItemScrolledIntoView(ListView, Item, Widget)
    MapListEntry(ListView, Item, Widget)
    NotifyInitializedWidget(ListView, Item, Widget)
end

-- 列表滑动事件
local function  OnListViewScrolled(ListView, ItemOffset, DistanceRemaining)

end

local function BindListViewDelegate(ListView, delegate, cb)
    delegate:Clear()
    delegate:Add(ListView, cb)
end

local function BindListView(ListView)
    local ListInfo = RequireListViewCache(ListView)
    if not ListInfo.bBindEvent then
        ListInfo.bBindEvent = true
        BindListViewDelegate(ListView, ListView.BP_OnEntryInitialized, OnEntryInitialized)    
        BindListViewDelegate(ListView, ListView.BP_OnItemClicked, OnItemClicked)
        BindListViewDelegate(ListView, ListView.BP_OnItemDoubleClicked, OnItemDoubleClicked)
        BindListViewDelegate(ListView, ListView.BP_OnItemIsHoveredChanged, OnItemIsHoveredChanged)
        BindListViewDelegate(ListView, ListView.BP_OnItemSelectionChanged, OnItemSelectionChanged)
        BindListViewDelegate(ListView, ListView.BP_OnItemScrolledIntoView, OnItemScrolledIntoView)
        BindListViewDelegate(ListView, ListView.BP_OnListViewScrolled, OnListViewScrolled)
    end
end

function _G.GlbListViewGetAnyWidget(ListView)    
    local ListInfo = ListMapInfos[ListView]
    if ListInfo then
        for Widget, item in pairs(ListInfo.Widget2Item) do
            return Widget
        end
    end
    return nil
end

function _G.GlbSelectListViewItem(ListView, Item)
    OnItemSelectionChanged(ListView, Item, true)    
end

-- 通用的列表刷新事件
---@param customInfo CustomData @自定义的数据，传给子节点初始化的
---@param ListView UListView @UListView
---@param entryItemClass UClass @EntryItemClass
---@param entryBindScript String @ script file name
---@param entyrSources TArray<any> @Source item info list, entrySource can also be lua Table
---@param copyObjectCb Callback @copy source object info to list litem,  copyObjectCb(item, sourceInfo, index)
function _G.GlbRefreshListView(customInfo, ListView, entryItemClass, entryBindScript, entyrSources, copyObjectCb)    
    BindListView(ListView)

    local worldContext = ListView:GetWorld()
    local sourceNum = #entyrSources
    local ListItems = ListView.ListItems
    local curEntryCount = #ListItems
    if curEntryCount < sourceNum then
        for i = curEntryCount + 1, sourceNum do
            local entryObject = NewObject(entryItemClass, worldContext, nil, entryBindScript)
            ListItems:Add(entryObject)
        end
    else
        for i = curEntryCount, sourceNum + 1, -1 do
            ListItems:Remove(i)
        end
    end

    if copyObjectCb then
        curEntryCount = #ListItems
        for i = 1, sourceNum do
            local entryObject = ListItems:Get(i)
            copyObjectCb(entryObject, entyrSources[i], i)
        end
    end
    ListMapInfos[ListView] = nil
    local ListInfo = RequireListViewCache(ListView)
    ListInfo.CustomInfo = customInfo
    ListView:RequestRefresh()
    ListView:RegenerateAllEntries()
end

-- 清除缓存
function _G.GlbClearListViewCache(ListView)
    ListMapInfos[ListView] = nil
end
