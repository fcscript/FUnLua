local UIPanelBase = {}
UIPanelBase.__index = UIPanelBase

function UIPanelBase:GetPanelName()
    return self.PlaneName
end

function UIPanelBase:SetPanelName(planelName)
    self.PlaneName = planelName
end

function UIPanelBase:SetLayer(layer)
    self.Layer = layer
end

function UIPanelBase:GetLayer()
    return self.Layer
end

return UIPanelBase