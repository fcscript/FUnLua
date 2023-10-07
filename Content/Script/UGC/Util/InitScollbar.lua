---@type InitScollbar

local function AddOrRefreshScollbarChild(customInfo, Scollbar, childIndex, source, childWigetClass, refreshFuncName)
    local widget = Scollbar:GetChildAt(childIndex)
    if widget == nil then
        widget = UE.UWidgetBlueprintLibrary.Create(Scollbar, childWigetClass ,nil)
        Scollbar:AddChild(widget)
    end
    if refreshFuncName == nil then
        refreshFuncName = "Refresh"
    end
    local refreshFunc = widget[refreshFuncName]
    if refreshFunc and type(refreshFunc) == "function" then
        refreshFunc(widget, customInfo, childIndex, source)
    else
        UnLua.Error("[Scrollbar]invalid function name:", refreshFuncName)
    end
end

-- 通用的列表刷新事件
---@param customInfo CustomData @自定义的数据，传给子节点初始化的
---@param Scollbar UScollbar @UScollbar
---@param childWigetClass UClass @childWigetClass
---@param entrySources List<any> @Source item info list
---@param refreshFuncName string @child widget refresh func name
function _G.GlbRefreshScollbar(customInfo, Scollbar, childWigetClass, entrySources, refreshFuncName)
    local childIndex = 0
    for key, value in pairs(entrySources) do
        AddOrRefreshScollbarChild(customInfo, Scollbar, childIndex, value, childWigetClass, refreshFuncName )
        childIndex = childIndex + 1
    end

    -- 删除多余的组件
    local totalChildCount = Scollbar:GetChildrenCount()
    while totalChildCount > childIndex do
        Scollbar:RemoveChildAt(childIndex)
        totalChildCount = totalChildCount - 1
    end
end
