require "UnLua.Script.UnLua"
local UMG_Profile = LuaUnrealClass()

function UMG_Profile:Construct()

    self:BindButton(self.ButtonClose, self.OnClickButtonClose)
    self:BindButton(self.ReadStr, self.OnClickReadStr)
    self:BindButton(self.ReadVector, self.OnClickReadVector)
    self:BindButton(self.ReadInt, self.OnClickReadInt)

    self:BindButton(self.WriteStr, self.OnClickWriteStr)
    self:BindButton(self.WriteVector, self.OnClickWriteVector)
    self:BindButton(self.WriteInt, self.OnClickWriteInt)
end

function UMG_Profile:BindButton(button, func)
    button.OnClicked:Clear()
    button.OnClicked:Add(self, handle(self, func))
end

function UMG_Profile:OnClickButtonClose()
    local UIManager = require "UnLua.Script.UI.UIManager"
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
	local color = FLinearColor()
	color.A = 1	
	color.G = 1
    UEPrint(StrLog)
	UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
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
    local value = _G.FVector(-1, 504, 82)
    self:DoWrite("WriteVector", "VectorValue", value)    
end

function UMG_Profile:OnClickWriteInt()
    local value = 1000
    self:DoWrite("WriteInt", "IntValue", value)   
end

return UMG_Profile
