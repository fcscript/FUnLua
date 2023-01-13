local ActorCallback2 = UnLua.Class()
-- local ActorCallback2 = {}

function ActorCallback2:Initialize()
end

function ActorCallback2:ReceiveBeginPlay()
    UEPrint("[ActorCallback2]ReceiveBeginPlay")
end

function ActorCallback2:NotifyIDList(OutIDList)
    UEPrint("[ActorCallback2]NotifyIDList, OutIDList=", OutIDList, ",Len=", #OutIDList)
    local len = #OutIDList
    for i = 1, len do
        UEPrint("[ActorCallback2]NotifyIDList, OutIDList[]=", OutIDList[i])
    end
    OutIDList:Add(100)
    OutIDList:Add(120)
    OutIDList:Add(150)
    return #OutIDList, OutIDList
end

function ActorCallback2:NotifyAvatarParam(AvatarParam)
    AvatarParam.MaleFaceConfigPath = "abc_path"
    AvatarParam.HideBoneWhiteList:Add("a1")
    AvatarParam.HideBoneWhiteList:Add("a2")
    AvatarParam.HideBoneWhiteList:Add("a3")
    return "lua modify", AvatarParam
end

function ActorCallback2:NotifyIDSet(OutIDSet)
    OutIDSet:Add(10)
    OutIDSet:Add(11)    
    return #OutIDSet, OutIDSet
end

function ActorCallback2:NotifyIDMap(OutIDMap)
    OutIDMap:Add(8, 80)
    OutIDMap:Add(9, 90)    
    return #OutIDMap, OutIDMap
end

function ActorCallback2:OnActorClick(TouchedActor, ButtonPressed)
    UEPrint("[ActorCallback2]OnActorClick, TouchedActor=", TouchedActor, ",ButtonPressed=", ButtonPressed)
    return self
end

function ActorCallback2:OnHit(SelfActor, OtherActor, NormalImpulse, Hit)
    UEPrint("[ActorCallback2]OnHit, NormalImpulse=", NormalImpulse, ",Hit=", Hit)
    return self
end

function ActorCallback2:OnTouchBegin(FingerIndex, TouchedActor)
    UEPrint("[ActorCallback2]OnTouchBegin, FingerIndex=", FingerIndex, ",TouchedActor=", TouchedActor)
    return self
end

function ActorCallback2:Touch_Pressed(FingerId, Location)
    UEPrint("[ActorCallback2]OnTouchBegin, FingerId=", FingerId, ",Location=", Location)
end

return ActorCallback2