local M = {}
M.__index = M

function M:TestCall(b1, n2, str3)
    print("[CallLua][TestCall]b1=", b1, ",n2=", n2, ",str3=", str3)
end

function M.TestCall2(b1, n2, str3)
    print("[CallLua][TestCall2]b1=", b1, ",n2=", n2, ",str3=", str3)
end

function _G.GlbCall(b1, n2, str3)
    print("[CallLua][GlbCall]b1=", b1, ",n2=", n2, ",str3=", str3)
end

return M