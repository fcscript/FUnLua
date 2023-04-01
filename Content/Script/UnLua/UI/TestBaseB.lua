local TestBaseB = UnLua.Class("UnLua.UI.TestBaseA")

function TestBaseB:Construct()
    -- UnLua.DebugBreak()
    -- self.Super.Construct(self)
    self.Super:Construct()
    self.Name = "TestBaseB"
    UEPrint("TestBaseB:Construct")
end

return TestBaseB