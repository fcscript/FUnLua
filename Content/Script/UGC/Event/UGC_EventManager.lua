-- UGC_EventManager

local M = 
{
    MsgListens = {},
    ListenID = 0
}
M.__index = M

function M:ListenUGCMessage(msgKey, thisPtr, cb)
    local msgListens = self.MsgListens[msgKey]
    if msgListens == nil then
        msgListens = {}
        self.MsgListens[msgKey] = msgListens
    end

    local listenID = self.ListenID + 1
    self.ListenID = listenID

    local len = #msgListens
    msgListens[len+1] = 
    {
        ListenID = listenID,
        ThisPtr = thisPtr,
        Cb = cb
    }
    return listenID
end

function M:UnListenUGCMessageByID(listenID)
    for msgKey, msgListens in pairs(self.MsgListens) do
        for i = #msgListens, 1, -1 do
            local node = msgListens[i]
            if node.ListenID == listenID then
                table.remove(msgListens, i)
            end
        end
    end
end

function M:UnListenUGCMessageByPtr(msgKey, thisPtr)
    if thisPtr == nil then
        return 
    end
    local msgListens = self.MsgListens[msgKey]
    if msgListens then
        for i = #msgListens, 1, -1 do
            local node = msgListens[i]
            if node.ThisPtr == thisPtr then
                table.remove(msgListens, i)
            end
        end
    end

end

function M:SendUGCMessage(msgKey, ...)
    local msgListens = self.MsgListens[msgKey]
    if msgListens then
        -- print("[UGC][SendUGCMessage]msgKey=", msgKey, ",listen count=", #msgListens)
        for i = 1, #msgListens do
            local node = msgListens[i]
            if node.ThisPtr then
                node.Cb(node.ThisPtr, ...)
            else
                node.Cb(...)
            end
        end
    else
        -- print("[UGC][SendUGCMessage]msgKey=", msgKey, ",none listen")
    end
end

return M