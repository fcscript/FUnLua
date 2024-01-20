
-- local MovingPanelUtil = _G.Class("MovingPanelUtil")
local MovingPanelUtil = {}
MovingPanelUtil.__index = MovingPanelUtil

local ScrollType = 
{
	LinearMovement = 0, -- 线性移动
	FastInAndStay = 1, -- 快速进入并且停留一会
}

function MovingPanelUtil:ctor(...)	
	self.Speed = 100
	self.InitSpeed = self.Speed
	self.ChildWidth = 200
	self.ChildHeight = 100
	self.ChildGap = 0
	self.ChildNum = 0
	self.TotalWidth = 200
	self.bNeedClip = true
	self.CanDrag = false
	self.bInit = false
	self.bFullPanel = true
	
	self.Left = 0
	self.Right = self.ChildWidth
	self.MoveX = 0
	self.bDragging = false	
	self.bAutoRestore = false
	
	self.DragStartTime = 0

	self.MoveType = ScrollType.FastInAndStay
	self.FastInTime = 0.5
	self.StayTime = 10
	self.bStayState = true
	self.StayIndex = 1  -- 停留的节点
	self.CurrentStayTime = 2  -- 当前节点需要停留的时长
end

function MovingPanelUtil.New(...)
	local instance = setmetatable({}, MovingPanelUtil)
	instance.__index = MovingPanelUtil
	instance:ctor(...)
	return instance	
end

function MovingPanelUtil:Init(ParentView, MovingPanel, Panel_Childs, ItemClass, ChildNum, ChildWidth, ChildHeight, ChildGap, bNeedClip)
	self.ParentView = ParentView
	self.MovingPanel = MovingPanel
	self.Panel_Childs = Panel_Childs

	self.ItemClass = ItemClass
	self.ChildNum = ChildNum
	self.ChildWidth = ChildWidth
	self.ChildHeight = ChildHeight
	self.ChildGap = ChildGap
	self.bNeedClip = bNeedClip
	self.bInit = true
	self.CanDrag = true
	self.bFullPanel = true
	self.bAutoRestore = false

	self:OnInitChilds()
end

-- 设置平滑移动模式
function MovingPanelUtil:SetLinearMovementMode(Speed)
	self.MoveType = ScrollType.LinearMovement
	self.Speed = Speed
end

-- 设置快速进入并停留一会的模式
function MovingPanelUtil:SetFastInStayMode(FastInTime, StayTime)
	self.MoveType = ScrollType.FastInAndStay
	self.FastInTime = FastInTime or 0.5
	self.StayTime = StayTime or 2
end

function MovingPanelUtil:OnInitChilds()
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
	local BPClass = UE4.UClass.Load(self.ItemClass)
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

	self.CurrentStayTime = self.StayTime	
	-- 重新计算移动速度
	if self.MoveType == ScrollType.FastInAndStay then
		self.Speed = self.ChildWidth / self.FastInTime
	end
	self.InitSpeed = self.Speed
end

function MovingPanelUtil:AdjustChildSize()
	if not self.bFullPanel then
		return 
	end
	if self.ChildWidth + 1 > self.PanelWidth then
		return 
	end
	local MinChildNum = self.ChildNum
	if MinChildNum < 2 then
		MinChildNum = 2
	end

	self.ChildWidth = self.PanelWidth
	self.ChildHeight = self.PanelHeight
	self.ChildWidthEx = self.ChildWidth + self.ChildGap
	self.TotalWidth = self.ChildWidthEx * MinChildNum	
	if self.bNeedClip then
		self.Left = self.ChildWidthEx * -1.0
	else
		self.Left = 0
	end
	self.Right = self.Left + self.TotalWidth
	
	local ChildCount = self.Panel_Childs:GetChildrenCount()
	for i = 1, ChildCount do		
		local Child = self.Panel_Childs:GetChildAt(i-1)
		local PanelSlot = Child.Slot
		local InOffset = UE4.FMargin()
		InOffset.Left = self.ChildWidthEx * (i - 1)
		InOffset.Top = 0
		InOffset.Right = self.ChildWidth
		InOffset.Bottom = self.ChildHeight
		PanelSlot:SetOffsets(InOffset)
	end
	
	-- 重新计算移动速度
	if self.MoveType == ScrollType.FastInAndStay then
		self.Speed = self.ChildWidth / self.FastInTime
	end
end

function MovingPanelUtil:UpdateMove(DeltaTime)
	if not self.bInit then
		return 
	end
	if not self.bDragging then
		-- 如果需要停留
		if self.MoveType == ScrollType.FastInAndStay then
			-- 停留阶段
			if self.bStayState then
				if self.ChildNum > 1 then
					self.CurrentStayTime = self.CurrentStayTime - DeltaTime
	
					if self.CurrentStayTime < 0 then
						self.bStayState = false
						self.CurrentStayTime = 0
					end
				end
			else
				-- 移动阶段
				self.MoveX = self.MoveX - self.Speed * DeltaTime
			end
		else
			-- 总是平滑移动
			self.MoveX = self.MoveX - self.Speed * DeltaTime
		end
		if self.ChildNum < 2 and not self.bAutoRestore then
			self.MoveX = 0
		end
	end
	self.MoveX = math.fmod(self.MoveX, self.TotalWidth)

	self:RefreshDragPos()
	local UpdateDeltaTime = self.UpdateDeltaTime + DeltaTime
	if UpdateDeltaTime > 1.0 then
		self.bInitPanelSize = false		
	end
	self.UpdateDeltaTime = UpdateDeltaTime
end

function MovingPanelUtil:RefreshDragPos()
	if not self.bInit then
		return 
	end
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
				self:AdjustChildSize()
			end
		end
	end
	local HalfWidth = self.HalfPanelWidth or 100
	local PanelCx = HalfWidth
	local HalfChildWidth = self.ChildWidth * 0.5

	local bNeedCheckStayIndex = not self.bStayState and not self.bDragging and self.MoveType == ScrollType.FastInAndStay

	local VisiableIndex = 1
	local MinDx = 10000
	local EnterWidget = nil
	for i = 1, self.ChildNum do
		local Child = self.Panel_Childs:GetChildAt(i-1)
		local X = self.ChildWidthEx * (i - 1) + self.MoveX
		X = self:ClipMove(X)
		
		local PanelSlot = Child.Slot
		if bNeedCheckStayIndex then
			local OldOffset = PanelSlot:GetOffsets()
			local OldX = OldOffset.Left
			local bNeedStay = false
			-- 从右向左移动
			if self.Speed > 0 and X < 0 and OldX > 0 and OldX < self.ChildWidth then
				bNeedStay = true
			elseif self.Speed < 0 and OldX < 0 and X > 0 then
				-- 从左向右移动
				bNeedStay = true
			end
			-- 需要在当前节点停留
			if bNeedStay then
				self.bAutoRestore = false
				self.bStayState = true
				self.StayIndex = i  -- 停留的节点
				self.CurrentStayTime = self.StayTime  -- 当前节点需要停留的时长
				self.Speed = self.InitSpeed
				
				VisiableIndex = i
				self.MoveX = -self.ChildWidthEx * (i - 1)
				X = 0
			end
		end		

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
		
		self.bStayState = false
		self.CurrentStayTime = 0  -- 当前节点需要停留的时长

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
	-- 自动靠齐功能，检查当前的位置
	local Child = self.Panel_Childs:GetChildAt(self.StayIndex-1)
	local PanelSlot = Child.Slot
	local Offset = PanelSlot:GetOffsets()
	local X = Offset.Left	
	if self.ChildNum == 1 then
		self.bAutoRestore = true
		if X > 0 then
			self.Speed = self.InitSpeed
		else
			self.Speed = -self.InitSpeed
		end
	else
		self.bAutoRestore = false
		if self.InitSpeed > 0 then
			if X > self.HalfPanelWidth then
				self.Speed = -self.InitSpeed
			end
		else
			if X < self.HalfPanelWidth then
				self.Speed = -self.InitSpeed
			end
		end
	end

	local NowTime = UE4.UGameplayStatics.GetTimeSeconds(self:GetWorld())
	if self.DragStartTime + 0.2 > NowTime then
		-- click事件
	end
end

return MovingPanelUtil