local M = {}
M.__index = M

function M.RefreshList(widgetList, ObjLists)
    local itemNum = widgetList:GetNumItems()
    local objsNum = #ObjLists

    local ListItems = widgetList.ListItems

    local comNum = math.min(itemNum, objsNum)
    for i = 1, comNum do
        ListItems:Set(i, ObjLists:Get(i))
    end

    if itemNum < objsNum then
        for i = itemNum + 1, objsNum do
            ListItems:Add(ObjLists:Get(i))
        end
    elseif itemNum > objsNum then
        for i = objsNum + 1, itemNum do
            ListItems:RemoveItem(i)
        end
    end

    widgetList:RequestRefresh()
    -- widgetList:RegenerateAllEntries()
end

return M