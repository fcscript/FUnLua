local ProfileFrame = {}
ProfileFrame.__index = ProfileFrame

local BuildParams = function (...)
    local args = {}
	for i = 1, select('#', ...) do 
        local arg = tostring(select(i, ...)) 
        args[i] = arg
    end  
    return table.concat(args, " ")
end

function ProfileFrame.PrintInfo(world, ...)
    local StrLog = BuildParams(...)
	local color = FLinearColor()
	color.A = 1	
	color.G = 1
    UEPrint(StrLog)
	UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
end

function ProfileFrame.GetHP(bpObj) 
    local StartTime = os.clock()
    local value = nil
    for i = 1, 100000 do
        value = bpObj:GetHP()
    end
    local costTime = os.clock() - StartTime
    local world = bpObj:GetWorld()
    ProfileFrame.PrintInfo(world, "[ProfileFrame]call function: GetHP, cost time:", costTime, ",value=", value)
end

function ProfileFrame.NotifyAll(world) 
    local vPos = FVector(1, 2, 3)
    local StartTime = os.clock()
    local value = nil
    for i = 1, 100000 do
        value = UFCTest.NotifyAll(0, vPos)
    end
    local costTime = os.clock() - StartTime
    ProfileFrame.PrintInfo(world, "[ProfileFrame]call function: NotifyAll, cost time:", costTime, ",value=", value)
end

function ProfileFrame.SetIDList(bpObj)
    local ids = UE4.TArray(_G.int32)
    for i = 1, 10 do
        ids:Add(i)
    end
    local StartTime = os.clock()
    for i = 1, 100000 do
        bpObj:SetIDList(ids)
    end
    local costTime = os.clock() - StartTime
    ProfileFrame.PrintInfo(world, "[ProfileFrame]call function: SetIDList, cost time:", costTime)
end

function ProfileFrame.DoStruct(world, avatarParam)    
    local StartTime = os.clock()
    for i = 1, 100000 do
        avatarParam.BoneAdjustItem.SlotName = "abc"
    end
    local costTime = os.clock() - StartTime

    ProfileFrame.PrintInfo(world, "[ProfileFrame]set struct member, cost time:", costTime, ",slotName:", avatarParam.BoneAdjustItem.SlotName)
end

return ProfileFrame