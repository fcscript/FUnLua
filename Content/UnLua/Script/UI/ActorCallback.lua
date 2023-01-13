local ActorCallback = UnLua.Class()
-- local ActorCallback = {}

function ActorCallback:Initialize()
end

function ActorCallback:ReceiveBeginPlay()
    UEPrint("[ActorCallback]ReceiveBeginPlay")
end

function ActorCallback:NotifyIDList(OutIDList)
    UEPrint("[ActorCallback]NotifyIDList, OutIDList=", OutIDList, ",Len=", OutIDList:Num())
    local len = OutIDList:Num()
    for i = 1, len do
        UEPrint("[ActorCallback]NotifyIDList, OutIDList[]=", OutIDList[i])
    end
    OutIDList:Add(10)
    OutIDList:Add(12)
    OutIDList:Add(15)
    return #OutIDList, OutIDList
end

function ActorCallback:NotifyAvatarParam(AvatarParam)
    AvatarParam.MaleFaceConfigPath = "abc_path"
    AvatarParam.HideBoneWhiteList:Add("a1")
    AvatarParam.HideBoneWhiteList:Add("a2")
    AvatarParam.HideBoneWhiteList:Add("a3")
    return "lua modify", AvatarParam
end

function ActorCallback:NotifyIDSet(OutIDSet)
    OutIDSet:Add(10)
    OutIDSet:Add(11)    
    return #OutIDSet, OutIDSet
end

function ActorCallback:NotifyIDMap(OutIDMap)
    OutIDMap:Add(8, 80)
    OutIDMap:Add(9, 90)    
    return #OutIDMap, OutIDMap
end

function ActorCallback:OnActorClick(TouchedActor, ButtonPressed)
    UEPrint("[ActorCallback]OnActorClick, TouchedActor=", TouchedActor, ",ButtonPressed=", ButtonPressed)
    return self
end

function ActorCallback:OnHit(SelfActor, OtherActor, NormalImpulse, Hit)
    UEPrint("[ActorCallback]OnHit, NormalImpulse=", NormalImpulse, ",Hit=", Hit)
    return self
end

function ActorCallback:OnTouchBegin(FingerIndex, TouchedActor)
    UEPrint("[ActorCallback]OnTouchBegin, FingerIndex=", FingerIndex, ",TouchedActor=", TouchedActor)
    return self
end

function ActorCallback:Touch_Pressed(FingerId, Location)
    UEPrint("[ActorCallback]OnTouchBegin, FingerId=", FingerId, ",Location=", Location)
end

function ActorCallback:Died_Brocast_RPC(InID)
    UEPrint("[ActorCallback]Died_Brocast_RPC, InID=", InID)
end

return ActorCallback