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
	local color = UE4.FLinearColor()
	color.A = 1	
	color.G = 1
    UEPrint(StrLog)
	UE4.UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
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
    local vPos = UE4.FVector(1, 2, 3)
    local StartTime = os.clock()
    local value = nil
    for i = 1, 100000 do
        value = UE4.UFCTest.NotifyAll(0, vPos)
    end
    local costTime = os.clock() - StartTime
    ProfileFrame.PrintInfo(world, "[ProfileFrame]call function: NotifyAll, cost time:", costTime, ",value=", value)
end

function ProfileFrame.SetIDList(bpObj)
    local ids = UE4.TArray(UE4.int32)
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

    ProfileFrame.PrintInfo(world, "[ProfileFrame]set struct member child, cost time:", costTime, ",slotName:", avatarParam.BoneAdjustItem.SlotName)
end

function ProfileFrame.DoStructOne(world, avatarParam)    
    local StartTime = os.clock()
    for i = 1, 100000 do
        avatarParam.MaleFaceConfigPath = "abc"
    end
    local costTime = os.clock() - StartTime

    ProfileFrame.PrintInfo(world, "[ProfileFrame]set struct member one, cost time:", costTime, ",MaleFaceConfigPath:", avatarParam.BoneAdjustItem.SlotName)
end

function ProfileFrame.Doblueprint_ReadInt(world, obj, inputValue)
    inputValue = inputValue or 101
    ProfileFrame.Doblueprint_ReadMember(world, obj, inputValue, "ID", "int")
end

function ProfileFrame.Doblueprint_ReadFString(world, obj, inputValue)
    inputValue = inputValue or "none_input"
    ProfileFrame.Doblueprint_ReadMember(world, obj, inputValue, "NameValue", "FString")
end

function ProfileFrame.Doblueprint_ReadFVector(world, obj, inputValue)
    if not inputValue then
        inputValue = UE4.FVector(1, 2, 8)
    end
    ProfileFrame.Doblueprint_ReadMember(world, obj, inputValue, "Pos", "FVector")
end

function ProfileFrame.Doblueprint_ReadMember(world, obj, inputValue, valueName, valueType)
    if inputValue then
        obj[valueName] = inputValue
    end
    local value = 0
    local StartTime = os.clock()
    for i = 1, 100000 do
        value = obj[valueName]
    end
    local costTime = os.clock() - StartTime
    ProfileFrame.PrintInfo(world, "[ProfileFrame]Doblueprint_ReadMember, type:",  valueType, ", cost time:",costTime, ",value:", value)
end

function ProfileFrame.Doblueprint_WriteMember(world, obj, inputValue, valueName, valueType)
    local StartTime = os.clock()
    for i = 1, 100000 do
        obj[valueName] = inputValue
    end
    local costTime = os.clock() - StartTime
    local value = obj[valueName]
    ProfileFrame.PrintInfo(world, "[ProfileFrame]Doblueprint_WriteMember, type:",  valueType, ", cost time:",costTime, ",value:", value)
end

function ProfileFrame.Doblueprint_WriteInt(world, obj, inputValue)
    inputValue = inputValue or 1000
    ProfileFrame.Doblueprint_WriteMember(world, obj, inputValue, "ID", "int")
end

function ProfileFrame.Doblueprint_WriteFString(world, obj, inputValue)
    inputValue = inputValue or "none_input_string"
    ProfileFrame.Doblueprint_WriteMember(world, obj, inputValue, "NameValue", "FString")
end

function ProfileFrame.Doblueprint_WriteFVector(world, obj, inputValue)
    if not inputValue then
        inputValue = UE4.FVector(8, 9, 10)
    end
    ProfileFrame.Doblueprint_WriteMember(world, obj, inputValue, "Pos", "FVector")
end

function ProfileFrame.Doblueprint_MemberTest(world, obj)    
    ProfileFrame.Doblueprint_ReadInt(world, obj)
    ProfileFrame.Doblueprint_ReadFString(world, obj)
    ProfileFrame.Doblueprint_ReadFVector(world, obj)

    ProfileFrame.Doblueprint_WriteInt(world, obj)
    ProfileFrame.Doblueprint_WriteFString(world, obj)
    ProfileFrame.Doblueprint_WriteFVector(world, obj)
end

return ProfileFrame