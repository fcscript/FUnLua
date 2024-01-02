
-- local MovingPanelUtil = _G.Class("MovingPanelUtil")
local MovingPanelUtil = {}
MovingPanelUtil.__index = MovingPanelUtil

function MovingPanelUtil:ctor(...)
	self.Speed = 100
	self.ChildWidth = 200
	self.ChildHeight = 100
	self.ChildGap = 0
	self.bNeedClip = true
	self.CanDrag = true
	
	self.Left = 0
	self.Right = self.ChildWidth
	self.MoveX = 0
	self.bDragging = false	
    print("[UGC][MovingPanelUtil]:ctor")
end

function MovingPanelUtil.New(...)
	local instance = setmetatable({}, MovingPanelUtil)
	instance.__index = MovingPanelUtil
	instance:ctor(...)
	return instance	
end

function MovingPanelUtil:Init(ParentView, MovingPanel, Panel_Childs, ItemClass, ChildNum, ChildWidth, ChildHeight, Speed, ChildGap, bNeedClip)
    print("[UGC][MovingPanelUtil]:Init")
	self.ParentView = ParentView
	self.MovingPanel = MovingPanel
	self.Panel_Childs = Panel_Childs

	self.ItemClass = ItemClass
	self.ChildNum = ChildNum
	self.ChildWidth = ChildWidth
	self.ChildHeight = ChildHeight
	self.Speed = Speed
	self.ChildGap = ChildGap
	self.bNeedClip = bNeedClip

	self:OnInitChilds()
end

function MovingPanelUtil:OnInitChilds()
    print("[UGC][MovingPanelUtil]:OnInitChilds")
	self.bDragging = false	
	self.Panel_Childs:ClearChildren()
	self.ChildWidthEx = self.ChildWidth + self.ChildGap
	self.TotalWidth = self.ChildWidthEx * self.ChildNum
	if self.bNeedClip then
		self.Left = self.ChildWidthEx * -1.0
	else
		self.Left = 0
	end
	self.Right = self.Left + self.TotalWidth

	-- UKismetSystemLibrary::LoadClassAsset_Blocking
	local BPClass = UClass.Load(self.ItemClass)
	for i = 1, self.ChildNum do		
		local BPWidget = UE4.UWidgetBlueprintLibrary.Create(self:GetWorld(), BPClass, nil)
		local PanelSlot = self.Panel_Childs:AddChild(BPWidget)
		local InOffset = UE4.FMargin()
		InOffset.Left = self.ChildWidthEx * (i - 1)
		InOffset.Top = 0
		InOffset.Right = self.ChildWidth
		InOffset.Bottom = self.ChildHeight
		PanelSlot:SetOffsets(InOffset)
	end

	self.UpdateDeltaTime = 0
end

function MovingPanelUtil:UpdateMove(DeltaTime)
	if not self.bDragging then
		self.MoveX = self.MoveX - self.Speed * DeltaTime
	end
	self.MoveX = math.fmod(self.MoveX, self.TotalWidth)

	self:RefreshDragPos()
end

function MovingPanelUtil:RefreshDragPos()
	-- 计算自己的大小
	if not self.bInitPanelSize then
		local PanelGeometry = self.MovingPanel:GetCachedGeometry()
		if PanelGeometry then
			local LocalSize = UE4.USlateBlueprintLibrary.GetLocalSize(PanelGeometry)
			if LocalSize.X > 0 then
				self.bInitPanelSize = true
				self.PanelWidth = LocalSize.X
				self.PanelHeight = LocalSize.Y
				self.HalfPanelWidth = self.PanelWidth / 2
			end
		end
	end
	local HalfWidth = self.HalfPanelWidth or 100
	local PanelCx = HalfWidth
	local HalfChildWidth = self.ChildWidth * 0.5

	local VisiableIndex = 1
	local MinDx = 10000
	local EnterWidget = nil
	for i = 1, self.ChildNum do
		local Child = self.Panel_Childs:GetChildAt(i-1)
		local X = self.ChildWidthEx * (i - 1) + self.MoveX
		X = self:ClipMove(X)
		
		local PanelSlot = Child.Slot
		-- local OldOffset = PanelSlot:GetOffsets()
		local InOffset = UE4.FMargin()
		InOffset.Left = X
		InOffset.Top = 0
		InOffset.Right = self.ChildWidth
		InOffset.Bottom = self.ChildHeight
		PanelSlot:SetOffsets(InOffset)

		local Cx = X + HalfChildWidth
		local Dx = math.abs(Cx - PanelCx)
		if Dx < MinDx then
			MinDx = Dx
			VisiableIndex = i
			EnterWidget = Child
		end
	end

	-- 看谁显示的最多，选取最多显示的节点
	if EnterWidget and self.bInitPanelSize then
		self:OnEnterItem(EnterWidget, VisiableIndex)
	end
end

function MovingPanelUtil:OnEnterItem(ChildWiget, Index)
	local OldEnterIndex = self.EnterIndex or -1
	if OldEnterIndex ~= Index then
		local OldWidget = self.EnterItem
		self.EnterIndex = Index
		self.EnterItem = ChildWiget
		print("[UGC][MovingPanelUtil]OnEnterItem, Index=", Index, ",PanelWidth=", self.PanelWidth)
		if OldWidget and self.ParentView.OnLeaveItem then
			self.ParentView:OnLeaveItem(OldWidget, OldEnterIndex)
		end
		if self.ParentView.OnEnterItem then
			self.ParentView:OnEnterItem(ChildWiget, Index)
		end
	end
end

function MovingPanelUtil:ClipMove(x)
	if self.bNeedClip then
		if x < self.Left then
			return x + self.TotalWidth
		elseif x > self.Right then
			return x - self.TotalWidth
		else
			return x
		end
	else
		return x
	end
end

function MovingPanelUtil:IsDragRange(MyGeometry, MouseEvent)
	local PanelGeometry = self.MovingPanel:GetCachedGeometry()
	local ScreenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)
	local LocalPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(PanelGeometry, ScreenPos)
	local LocalSize = UE4.USlateBlueprintLibrary.GetLocalSize(PanelGeometry)
	-- print("[UGC][MovingPanelUtil],LocalPos:", LocalPos, ",LocalSize:", LocalSize)
	local X = LocalPos.X
	local Y = LocalPos.Y
	local W = LocalSize.X
	local H = LocalSize.Y
	return X > 0 and X < W and Y > 0 and Y < H
end

function MovingPanelUtil:GetWorld()
	if _G.GetCurrentWorld then
		return _G.GetCurrentWorld()
	end
	return self.ParentView:GetWorld()
end

function MovingPanelUtil:OnDragStart(MyGeometry, MouseEvent)
    print("[UGC][MovingPanelUtil]:OnDragStart, CanDrag=", self.CanDrag)
	if not self.CanDrag then
		return
	end
	self.bDragging = true

	local ScreenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)
	self.LMBDownScreenPos = ScreenPos
	local LocalPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(MyGeometry, ScreenPos)
	self.LMBDownLocalPos = UE4.FVector2D(LocalPos.X, LocalPos.Y)
	self.LastLocalPos = UE4.FVector2D(LocalPos.X, LocalPos.Y)
	self.DragStartTime = UE4.UGameplayStatics.GetTimeSeconds(self:GetWorld())
	self.ParentView:SetFocus()
end

function MovingPanelUtil:OnDragging(MyGeometry, MouseEvent)
	if not self.bDragging then
		return 
	end
    -- print("[UGC][MovingPanelUtil]:OnDragging, bDragging=", self.bDragging)
	local LastLocalPos = self.LastLocalPos
	local ScreenPos = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)
	local CurLocalPos = UE4.USlateBlueprintLibrary.AbsoluteToLocal(MyGeometry, ScreenPos)
	self.LastLocalPos = UE4.FVector2D(CurLocalPos.X, CurLocalPos.Y)
	local MoveX = CurLocalPos.X - LastLocalPos.X
	local MoveY = CurLocalPos.Y - LastLocalPos.Y
	local Abs_X = math.abs(MoveX)
	local Abs_Y = math.abs(MoveY)

	-- 横向托动
	if Abs_X > Abs_Y then
		self.MoveX = self.MoveX + MoveX
		self.MoveX = math.fmod(self.MoveX, self.TotalWidth)
		if self.bNeedClip then
			self:RefreshDragPos()
		else
			self:RefreshDragPos()
		end
	else
		-- 纵向托动
	end
end

function MovingPanelUtil:OnDragEnd(MyGeometry, MouseEvent)
	self.bDragging = false
	local NowTime = UE4.UGameplayStatics.GetTimeSeconds(self:GetWorld())
	if self.DragStartTime + 0.2 > NowTime then
		-- click事件
	end
end

return MovingPanelUtil