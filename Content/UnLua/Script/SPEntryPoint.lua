------------------------------------------
--- 游戏的入口
---
---
------------------------------------------
------------------------------------------
------------------------------------------
require "UnLua.Script.UnLua"
require "UnLua.Script.SPParallelGlobalFuncDefine"

function  _G.SPWorldAfterInit(world, gameInstance)
end

function _G.SPTickMain(deltaTime, now)
end

-- PIE相关接口
function _G.PreBeginPIE(bIsSimulating)
end

function _G.BeginPIE(bIsSimulating)
end

function _G.PostPIEStarted(bIsSimulating)
end

function _G.PrePIEEnded(bIsSimulating)
end

function _G.EndPIE(bIsSimulating)
end
-- PIE相关接口