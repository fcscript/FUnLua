---@type UMG_Main_C
local M = UnLua.Class()

function M:Construct()
	self.ExitButton.OnClicked:Add(self, M.OnClicked_ExitButton)
	self.ButtonKillSelf.OnClicked:Add(self, M.OnClicked_KillSelf)	
	self.ButtonReload.OnClicked:Add(self, M.OnClicked_Reload)
	--self.ExitButton.OnClicked:Add(self, function(Widget) UE.UKismetSystemLibrary.ExecuteConsoleCommand(Widget, "exit") end )
end

function M:OnClicked_ExitButton()
	UE.UKismetSystemLibrary.ExecuteConsoleCommand(self, "exit")
end

function M:OnClicked_KillSelf()	
	local worldContext = self
	local localPlayerControler = UE4.UGameplayStatics.GetPlayerController(worldContext, 0)
	local actor = localPlayerControler.Pawn
	UE.UGameplayStatics.ApplyDamage(actor, 200, localPlayerControler, actor, nil)
end

function M:OnClicked_Reload()	
	UE.UKismetSystemLibrary.ExecuteConsoleCommand(self, "RestartLevel")
end

return M
