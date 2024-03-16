-- UGC_SelectInfo
local M = 
{
    SelectObjects = {}, -- 选中的对象，可以是多个
    Transfrom_Mode = 0, -- 托动模式
    NeedClearSelectAxis = 0, -- 
    SelectAxis = 0, -- 选中的轴
    SelectRotations = {}, -- 选中对象的托动前的旋转
    SelectTransforms = {}, 
}

return M