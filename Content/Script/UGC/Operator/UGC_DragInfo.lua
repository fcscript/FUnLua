-- UGC_DragInfo
local M = 
{
    DragItem = nil,  -- 托动的组件对象(从UI上托出来的)
    WorldItem = nil, -- 已经在场景中创建中托放的组件对象
    MainRoot = nil,
}
M.__index = M

return M