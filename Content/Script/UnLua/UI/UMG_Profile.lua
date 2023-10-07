-- require "UnLua.UnLua"
local UMG_Profile = UnLua.Class()

function UMG_Profile:Construct()

    self:BindButton(self.ButtonClose, self.OnClickButtonClose)
    self:BindButton(self.ReadStr, self.OnClickReadStr)
    self:BindButton(self.ReadVector, self.OnClickReadVector)
    self:BindButton(self.ReadInt, self.OnClickReadInt)

    self:BindButton(self.WriteStr, self.OnClickWriteStr)
    self:BindButton(self.WriteVector, self.OnClickWriteVector)
    self:BindButton(self.WriteInt, self.OnClickWriteInt)

    self:BindButton(self.ArrayInt, self.OnClickArrayInt)
    self:BindButton(self.ArrayByte, self.OnClickArrayByte)
    self:BindButton(self.ArrayStr, self.OnClickArrayStr)
    
    self:BindButton(self.Other, self.DoOther)
end

function UMG_Profile:BindButton(button, func)
    button.OnClicked:Clear()
    button.OnClicked:Add(self, func)
end

function UMG_Profile:OnClickButtonClose()
    local UIManager = require "UnLua.UI.UIManager"
    UIManager.HidePanel(self)
end

local BuildParams = function (...)
    local args = {}
	for i = 1, select('#', ...) do 
        local arg = tostring(select(i, ...)) 
        args[i] = arg
    end  
    return table.concat(args, " ")
end

function UMG_Profile:PrintInfo(...)
    local StrLog = BuildParams(...)
    local world = self:GetWorld()    
	local color = UE.FLinearColor()
	color.A = 1	
	color.G = 1
    UEPrint(StrLog)
	UE4.UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
end

function UMG_Profile:DoRead(funcName, valueName)
    local StartTime = os.clock()
    local value = nil
    for i = 1, 100000 do
        value = self[valueName]
    end
    local costTime = os.clock() - StartTime
    self:PrintInfo("[Unlua]test read ", funcName, ", cost time:", costTime, ",value=", value)
end

function UMG_Profile:DoWrite(funcName, valueName, value)
    local StartTime = os.clock()
    for i = 1, 100000 do
        self[valueName] = value
    end
    local costTime = os.clock() - StartTime
    self:PrintInfo("[Unlua]test write ", funcName, ", cost time:", costTime, ",write value=", self[valueName])
end

function UMG_Profile:OnClickReadStr()
    self:DoRead("ReadStr", "StrValue")
end

function UMG_Profile:OnClickReadVector()
    self:DoRead("ReadVector", "VectorValue")    
end

function UMG_Profile:OnClickReadInt()
    self:DoRead("ReadInt", "IntValue")    
end

function UMG_Profile:OnClickWriteStr()
    local value = "aaaaaaaaaaa";
    self:DoWrite("WriteStr", "StrValue", value)
end

function UMG_Profile:OnClickWriteVector()
    local value = UE4.FVector(-1, 504, 82)
    self:DoWrite("WriteVector", "VectorValue", value)    
end

function UMG_Profile:OnClickWriteInt()
    local value = 1000
    self:DoWrite("WriteInt", "IntValue", value)   
end

function UMG_Profile:DoArrayGet(funcName, testArray)    
    local len = testArray:Length() - 1
    local StartTime = os.clock()
    local value = nil
    for i = 1, 100000 do
        local index = math.modf(i%len)
        value = testArray:Get(index+1)
    end
    local costTime = os.clock() - StartTime
    self:PrintInfo("[Unlua]Array read ", funcName, ", cost time:", costTime, ",value=", value, ",Len=", len)
end

function UMG_Profile:DoArraySet(funcName, testArray, value)    
    local len = testArray:Length() - 1
    local StartTime = os.clock()
    for i = 1, 100000 do
        local index = math.modf(i%len)
        testArray:Set(index+1, value)
    end
    local costTime = os.clock() - StartTime
    self:PrintInfo("[Unlua]Array Wrtie ", funcName, ", cost time:", costTime, ",value=", value, ",Len=", len)
end

function UMG_Profile:OnClickArrayInt()
    local testArray = UE4.TArray(UE.int32)
    for i = 1, 10 do 
        testArray:Add(i)
    end
    self:DoArrayGet("ArrayInt", testArray)
    self:DoArraySet("ArrayInt", testArray, 10)
end

function UMG_Profile:OnClickArrayByte()
    local testArray = UE4.TArray(UE.int8)
    for i = 1, 10 do 
        testArray:Add(i)
    end
    self:DoArrayGet("ArrayByte", testArray)
    self:DoArraySet("ArrayByte", testArray, 10)
end

function UMG_Profile:OnClickArrayStr()
    local testArray = UE4.TArray("")
    for i = 1, 10 do 
        testArray:Add(tostring(i))
    end
    self:DoArrayGet("ArrayStr", testArray)
    self:DoArraySet("ArrayStr", testArray, "aaa")
end

function UMG_Profile:DoOther()
    -- local AvatarClass = UE.UClass.Load("UFCTest")
    -- local AvatarClass = UE.UClass.Load("FCTest")
    local AvatarClass = UE4.UFCTest
    local obj = NewObject(AvatarClass)
    local objAvatar = obj.AvatarParams
    local ProfileFrame = require "FUnLua.ProfileFrame"
    local world = self:GetWorld()
    print("[ProfileFrame]UnLua------------------")
    ProfileFrame.DoStruct(world, objAvatar)
    ProfileFrame.DoStructOne(world, objAvatar)
    
    obj.HP = 101
    ProfileFrame.GetHP(obj)
    ProfileFrame.NotifyAll(obj)
    ProfileFrame.SetIDList(obj)
    print("[ProfileFrame]UnLua------------------")

    ProfileFrame.Doblueprint_MemberTest(world, obj)
end

return UMG_Profile
