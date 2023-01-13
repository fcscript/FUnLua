local UIManager = {}
UIManager.__index = UIManager
UIManager.PtrPanelList = {}
UIManager.Name2PanelList = {}
UIManager.StartID = 1

function UIManager.Reset(WorldContext)
    local self = UIManager
    local rootClassName = "/Game/UMG/UMG_Root.UMG_Root_C"
    local rootPanel = _G.LoadUserWidget(WorldContext, rootClassName, nil)
    self.RootPanel = rootPanel
    self.Root = rootPanel.Root
    self.StartLayer = 1
    rootPanel:AddToViewport(5000)
        
    self.PtrPanelList = {}
    self.Name2PanelList = {}
    self.StartID = 1
end

function UIManager.OpenPanel(WorldContext, className)
    local self = UIManager
    if self.Root == nil then
        local rootClassName = "/Game/UMG/UMG_Root.UMG_Root_C"
        local rootPanel = _G.LoadUserWidget(WorldContext, rootClassName, nil)
        self.RootPanel = rootPanel
        self.Root = rootPanel.Root
        self.StartLayer = 1
        rootPanel:AddToViewport(5000)        
    end    
    local panel = _G.LoadUserWidget(WorldContext, className, nil)
    if panel == nil then
        return 0
    end
    local parent = self.Root
    self.StartID = self.StartID + 1
    self.StartLayer = self.StartLayer + 1
    local panelInfo = 
    {
        Panel = panel,
        Layer = self.StartLayer,
        ID = self.StartID,
        Parent = parent,
        ClassName = className,
    }
    local addr = panel:GetObjectAddr()
    local slot = parent:AddChild(panel)
    self.PtrPanelList[addr] = panelInfo
    local panelList = self.Name2PanelList[className]
    if panelList == nil then
        panelList = {}
        self.Name2PanelList[className] = panelList
    end
    table.insert(panelList, panelInfo)

    panel:SetVisibility(_G.ESlateVisibility.SelfHitTestInvisible)

    print("[FCTestScript][UIManager]OpenPanel, ClassName=", className, ",StartID=", self.StartID, ",slot name:", slot:GetName(), ",panel=", panel, ",addr=", addr)

    UIManager.SetPanelAnchors(slot)
    
    return panel
end

function UIManager.SetPanelAnchors(slot)
    local zero = FVector2D()
    local anchors = FAnchors()
    anchors.Minimum = zero
    anchors.Maximum = FVector2D(1, 1)
    slot:SetAnchors(anchors)
    slot:SetSize(zero)
    slot:SetPosition(zero)
end

function UIManager.FindPanel(className)
    local self = UIManager
    local panels = UIManager.Name2Panels[className]
    if panels then
    end
end

function UIManager.HidePanel(panel)
    if not panel then
        return 
    end
    local addr = panel:GetObjectAddr()
    local object = panel:GetBPObject()
    print("[FCTestScript][UIManager]HidePanel, panel:", panel, ",object:", object, ",addr=", addr)
    local self = UIManager
    local panelInfo = self.PtrPanelList[addr]
    if panelInfo == nil then
        return 
    end

    self.PtrPanelList[addr] = nil
    local panelList = self.Name2PanelList[panelInfo.ClassName]
    if panelList then
        for i = 1, #panelList do
            if panelList[i].Panel == panel then
                table.remove(panelList, i)
                break
            end
        end
    end

    local index = panelInfo.Parent:GetChildIndex(panelInfo.Panel)
    panelInfo.Parent:RemoveChildAt(index)
    -- panel:Destory()
end

return UIManager