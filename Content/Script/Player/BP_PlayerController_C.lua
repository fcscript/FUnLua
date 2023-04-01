---@type BP_PlayerController_C
local M = UnLua.Class()

function M:UserConstructionScript()
	self.ForwardVec = UE.FVector()
	self.RightVec = UE.FVector()
	self.ControlRot = UE.FRotator()

	self.BaseTurnRate = 45.0
	self.BaseLookUpRate = 45.0
	UEPrint("[PlayerController]UserConstructionScript, Name:", self:GetName(), ",ForwardVec=", self.ForwardVec)
end

function M:ReceiveBeginPlay()
	-- UnLua.Ref(nil)
	if self:IsLocalPlayerController() then
		-- 创建一个准星界面
		local Widget = UE.UWidgetBlueprintLibrary.Create(self, UE.UClass.Load("/Game/Core/UI/UMG_Main.UMG_Main_C"))
		Widget:AddToViewport()
						
		-- 创建一个HUD界面
		-- local worldContext = self
		-- local localPlayerControler = UE4.UGameplayStatics.GetPlayerController(worldContext, 0)
		-- local localPlayer = localPlayerControler.Player

		-- local bpClassPath = "/Game/Core/UI/HUDLayer.HUDLayer_C"
		-- local widgetClass = UE.UClass.Load(bpClassPath)
		-- local wigetPanel = UE.UComUIExtendLibrary.PushContentToLayer_ForPlayer_ByClass(localPlayer, "UI.Layer.Game", widgetClass)

		-- self:SetupHUD()
	end

	self.Overridden.ReceiveBeginPlay(self)
end

function M:SetupHUD()
	local worldContext = self
	local localPlayerControler = UE4.UGameplayStatics.GetPlayerController(worldContext, 0)

	local hud = localPlayerControler:GetHUD()
	local bpHUDPath = "/Game/Core/UI/HUD.HUD_C"
	local hudBPClass = UE.UClass.Load(bpHUDPath)
	local world = self:GetWorld()
		
	-- local Transform = UE.FTransform()
	-- local AlwaysSpawn = UE.ESpawnActorCollisionHandlingMethod.AlwaysSpawn
	-- local hudObject = world:SpawnActor(hudBPClass, Transform, AlwaysSpawn, world, world)
	-- UEPrint("[HUD]hudBPClass=", hudBPClass, ",hudObject=", hudObject)

	local hud_class = hud and UE4.UGameplayStatics.GetObjectClass(hud) or nil
	if hud_class ~= hudBPClass then
		localPlayerControler:ClientSetHUD(hudBPClass)
		hud = localPlayerControler:GetHUD()
		UEPrint("[HUD]SetupHUD=", hudBPClass, ",old hud=", hud)
	end	
end

function M:Turn(AxisValue)
	-- UEPrint("[PlayerControl]Turn, AxisValue:", AxisValue)
	self:AddYawInput(AxisValue)
	
	-- local Rotation = self:GetControlRotation(self.ControlRot)
	-- Rotation:Set(0, Rotation.Yaw, 0)
	-- local Direction = Rotation:ToVector(self.ForwardVec)		-- Rotation:GetForwardVector()
	-- UEPrint("[PlayerControl]MoveForward, AxisValue:", AxisValue, ",Rotation=", Rotation)
	-- Rotation = self:GetControlRotation(self.ControlRot)
end

function M:TurnRate(AxisValue)
	local DeltaSeconds = UE.UGameplayStatics.GetWorldDeltaSeconds(self)
	local Value = AxisValue * DeltaSeconds * self.BaseTurnRate
	self:AddYawInput(Value)
end

function M:LookUp(AxisValue)
	-- UEPrint("[PlayerControl]LookUp, AxisValue:", AxisValue)
	self:AddPitchInput(AxisValue)
end

function M:LookUpRate(AxisValue)
	local DeltaSeconds = UE.UGameplayStatics.GetWorldDeltaSeconds(self)
	local Value = AxisValue * DeltaSeconds * self.BaseLookUpRate
	self:AddPitchInput(Value)
end

function M:MoveForward(AxisValue)
	if self.Pawn then
		local Rotation = self:GetControlRotation(self.ControlRot)
		Rotation:Set(0, Rotation.Yaw, 0)
		local Direction = Rotation:ToVector(self.ForwardVec)		-- Rotation:GetForwardVector()
		-- UEPrint("[PlayerControl]MoveForward, AxisValue:", AxisValue, ",Rotation=", Rotation, ",Direction=", Direction, ",ForwardVec=", self.ForwardVec)
		self.Pawn:AddMovementInput(Direction, AxisValue)
	end
end

function M:MoveRight(AxisValue)
	if self.Pawn then
		local Rotation = self:GetControlRotation(self.ControlRot)
		Rotation:Set(0, Rotation.Yaw, 0)
		local Direction = Rotation:GetRightVector(self.RightVec)
		self.Pawn:AddMovementInput(Direction, AxisValue)
	end
end

function M:Fire_Pressed()
	if self.Pawn then
		self.Pawn:StartFire_Server()
	else
		UE.UKismetSystemLibrary.ExecuteConsoleCommand(self, "RestartLevel")
	end
end

function M:Fire_Released()
	if self.Pawn then
		self.Pawn:StopFire_Server()
	end
end

function M:Aim_Pressed()
	if self.Pawn then
		-- local BPI_Interfaces = UE.UClass.Load("/Game/Core/Blueprints/BPI_Interfaces.BPI_Interfaces_C")
		-- BPI_Interfaces.UpdateAiming(self.Pawn, true)
		self.Pawn:UpdateAiming(true)
	end
end

function M:Aim_Released()
	if self.Pawn then
		-- local BPI_Interfaces = UE.UClass.Load("/Game/Core/Blueprints/BPI_Interfaces.BPI_Interfaces_C")
		-- BPI_Interfaces.UpdateAiming(self.Pawn, false)
		self.Pawn:UpdateAiming(false)
	end
end

function M:GetDoorComponent(actor)
	local component = actor:GetComponentByClass(UE4.UDoorBaseCompoment)
	if component == nil then
		component = actor:GetComponentByClass(UE4.UDoorRotationCompoment)
	end
	return component
end

function M:OnKeyOpenDoor_Released()
	local doorActor = self:FindFirstActor("Door1")
	UEPrint("[PlayerController]OnKeyOpenDoor, DoorCompoment, doorActor:", doorActor)
	 
    local world = self:GetWorld()
    local results = UE4.TArray(UE4.AActor)
    UE4.UGameplayStatics.GetAllActorsWithTag(world, "Door1", results)
	if results then
		local num = results:Num()
		for i = 1, num do
			local actor = results[i]
			local component = self:GetDoorComponent(actor)
			UEPrint("[PlayerController]OnKeyOpenDoor, PhysicsDoorCompoment, doorActor:", actor:GetName(), ",component=", component)
			if component then
				component:OpenDoor()
			end
		end
	else
		UEPrint("[PlayerController]OnKeyOpenDoor, not find door")
	end

	-- if doorActor then
	-- 	doorActor:OnKeyOpenDoor()
	-- end
end

function M:OnKeyCloseDoor_Released()
	UEPrint("[PlayerController]OnKeyCloseDoor")
	-- local doorActor = self:FindFirstActor("Door1")
	-- if doorActor then
	-- 	doorActor:OnKeyCloseDoor()
	-- end	
    local world = self:GetWorld()
    local results = UE4.TArray(UE4.AActor)
    UE4.UGameplayStatics.GetAllActorsWithTag(world, "Door1", results)
	if results then
		local num = results:Num()
		for i = 1, num do
			local actor = results[i]
			local component = self:GetDoorComponent(actor)
			if component then
				component:CloseDoor()
			end
		end
	end
end

function M:OnKeyAddImpulse_Released()
	UEPrint("[PlayerController]OnKeyAddImpulse")
	local doorActor = self:FindFirstActor("Door1")
	if doorActor then
		doorActor:OnKeyAddImpulse()
	end
end

function M:OnKeySubImpulse_Released()
	UEPrint("[PlayerController]OnKeySubImpulse")
	local doorActor = self:FindFirstActor("Door1")
	if doorActor then
		doorActor:OnKeySubImpulse()
	end
end

function M:FindFirstActor(tagName)    
    local world = self:GetWorld()
    local results = UE4.TArray(UE4.AActor)
    UE4.UGameplayStatics.GetAllActorsWithTag(world, tagName, results)    
    if results == nil then
        UEPrint("[PlayerController]FindFirstActor, results is nil")
        return nil
    end
    UEPrint("[PlayerController]FindFirstActor, results Numb:", results:Num())
    return results[1]
end

return M
