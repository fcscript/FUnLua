local FCPrint = {}

function FCPrint.PrintRefInfo(worldContext)
    local world = worldContext:GetWorld()
    local TotalBindCount = _G.GetBindObjectCount()
    local TotalRef = _G.GetTotalObjRef()
    local TotalIntPtr = _G.GetTotalIntPtr()
    local StrLog = "[FCTestScript]Total bind object:" .. TotalBindCount .. ",RefObj:" .. TotalRef .. ",IntPtr:" .. TotalIntPtr
    local color = FLinearColor()
    color.A = 1	
    color.G = 1
    UEPrint(StrLog)
    UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
end

local BuildParams = function (...)
    local args = {}
	for i = 1, select('#', ...) do 
        local arg = tostring(select(i, ...)) 
        args[i] = arg
    end  
    return table.concat(args, " ")
end

function FCPrint.PrintInfo(worldContext, ...)
    local StrLog = BuildParams(...)
    local world = worldContext:GetWorld()    
	local color = FLinearColor()
	color.A = 1	
	color.G = 1
    UEPrint(StrLog)
	UKismetSystemLibrary.PrintString(world, StrLog, true, false, color, 3.0)
end

return FCPrint
