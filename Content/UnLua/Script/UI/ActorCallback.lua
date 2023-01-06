local ActorCallback = UnLua.Class()

function ActorCallback:Initialize()
end

function ActorCallback:ReceiveBeginPlay()
    UEPrint("[ActorCallback]ReceiveBeginPlay")
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

return ActorCallback