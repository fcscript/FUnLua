local TestBaseC = UnLua.Class("UnLua.UI.TestBaseB")

function TestBaseC:Construct()
    UEPrint("TestBaseC:Construct--begin")
    self.Super:Construct()
    self.Name = "TestBaseC"
    UEPrint("TestBaseC:Construct--end")
end

function TestBaseC:Initialize(Initializer)
    self:Construct()
end


return TestBaseC