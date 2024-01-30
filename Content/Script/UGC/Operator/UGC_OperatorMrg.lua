-- UGC_OperatorMrg
local M = {}
M.__index = M

function M:Init(GameInstance)
    self.GameInstance = GameInstance
    local EventManager = _G.UGC.EventManager
    EventManager:ListenUGCMessage("UGC.Event.AddObject", self, M.OnAddObject)
    EventManager:ListenUGCMessage("UGC.Event.DeleteSelects", self, M.DeleteSelects)
    EventManager:ListenUGCMessage("UGC.Event.SelectObject", self, M.SelectObject)
    EventManager:ListenUGCMessage("UGC.Event.TransformSelectObject", self, M.TransformSelectObject)
    EventManager:ListenUGCMessage("UGC.Event.ChangeTransfromMode", self, M.ChangeTransfromMode)
end

function M:OnAddObject(Object)
end

function M:DeleteSelects(SelectObjects)
    self:HideAxisActor()
end

function M:SelectObject(SelectObj)
    if SelectObj then
        self:ShowAxisActor(SelectObj)
        self:ChangeTransfromMode()
    else
        self:HideAxisActor()
    end
end

function M:SelectObjectFromScene(SelectObj)
    if UE.UObject.IsValid(SelectObj) then        
        self:ShowAxisActor(SelectObj)
        _G.UGC.SelectInfo.SelectObjects = { SelectObj }
        self:ChangeTransfromMode()
        _G.UGC.EventManager:SendUGCMessage("UGC.Event.SelectObjectFromScene", SelectObj)
    else
        self:HideAxisActor()
        _G.UGC.SelectInfo.SelectObjects = {}
        _G.UGC.EventManager:SendUGCMessage("UGC.Event.SelectObject", nil)
    end
end

function M:TransformSelectObject()
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    local Location = UE.FVector()
    for i = 1, #SelectObjects do
        local SelectObject = SelectObjects[i]
        
        local Origin = UE.FVector()
        local BoxExtent = UE.FVector()
        SelectObject:GetActorBounds(true, Origin, BoxExtent, true)        
        Location = Location + Origin
    end
    local Count = #SelectObjects
    if Count > 0 then
        local Scale = 1.0 / Count
        Location = Location * Scale
    end
    if self.AxisActor then
        local SweepHitResult = UE.FHitResult()
        self.AxisActor:K2_GetRootComponent():K2_SetRelativeLocation(Location, false, SweepHitResult, false)
    end
end

-- 得到第一个选中对象的包围盒信处
function M:GetFirstSelectBoundBoxInfo()
    -- GetActorBounds
    local Origin = UE.FVector()
    local BoxExtent = UE.FVector()
    local Scale = UE.FVector(1, 1, 1)
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    local FirstObject = SelectObjects[1]
    if FirstObject then
        FirstObject:GetActorBounds(true, Origin, BoxExtent, true)
        Scale = FirstObject:GetActorScale3D()
    end
    local Info = { Origin = Origin, BoxExtent = BoxExtent, Scale = Scale }
    return Info
end

function M:ChangeTransfromMode()
    if self.AxisActor == nil then
        return 
    end
    local Transfrom_Mode = _G.UGC.SelectInfo.Transfrom_Mode
    local OperatorType = _G.UGC.OperatorType

    local bShowMove = false
    local bShowBox = false
    local bShowRotation = false
    -- 平移
    if Transfrom_Mode == OperatorType.Tf_Move then
        -- 隐藏旋转
        bShowMove = true
        self.AxisActor.TransfromComponentX.Length = 200
        self.AxisActor.TransfromComponentY.Length = 200
        self.AxisActor.TransfromComponentZ.Length = 200

        self.AxisActor.TransfromComponentX.Color = UE.FLinearColor(0, 0, 1, 1)
        self.AxisActor.TransfromComponentY.Color = UE.FLinearColor(0, 1, 0, 1)
        self.AxisActor.TransfromComponentZ.Color = UE.FLinearColor(1, 0, 0, 1)
        
        self.AxisActor.TransfromComponentX.Thickness = 2
        self.AxisActor.TransfromComponentY.Thickness = 2
        self.AxisActor.TransfromComponentZ.Thickness = 2
    elseif Transfrom_Mode == OperatorType.Tf_Rotaion then
        -- 旋转
        bShowRotation = true

        self.AxisActor.RotationComponentX.Thickness = 2
        self.AxisActor.RotationComponentY.Thickness = 2
        self.AxisActor.RotationComponentZ.Thickness = 2
    elseif Transfrom_Mode == OperatorType.Tf_Scale then
        -- 缩放
        bShowMove = true

        self.AxisActor.TransfromComponentX.Length = 200
        self.AxisActor.TransfromComponentY.Length = 200
        self.AxisActor.TransfromComponentZ.Length = 200
        
        self.AxisActor.TransfromComponentX.Color = UE.FLinearColor(0, 0.2, 0.8, 1)
        self.AxisActor.TransfromComponentY.Color = UE.FLinearColor(0.2, 0.8, 0, 1)
        self.AxisActor.TransfromComponentZ.Color = UE.FLinearColor(0.8, 0, 0.2, 1)

        self.AxisActor.TransfromComponentX.Thickness = 4
        self.AxisActor.TransfromComponentY.Thickness = 4
        self.AxisActor.TransfromComponentZ.Thickness = 4
    else
        bShowBox = true
        
        self.AxisActor.BoxComponent.Boxs:Clear()
        
        local SelectObjects = _G.UGC.SelectInfo.SelectObjects
        local Origin = UE.FVector()
        local BoxExtent = UE.FVector()

        local Box = UE.FBox()
        Box.IsValid = true
        for _idx, SelectObj in pairs(SelectObjects) do
            SelectObj:GetActorBounds(true, Origin, BoxExtent, true)
            Box.Min = Origin - BoxExtent
            Box.Max = Origin + BoxExtent
            self.AxisActor.BoxComponent.Boxs:Add(Box)
        end
        self.AxisActor.BoxComponent.bShowBigBox = true
        self.AxisActor.BoxComponent:UpdateBounds()        
        print("[UGC]ChangeTransfromMode, Transfrom_Mode=", Transfrom_Mode, ",bShowBox=", bShowBox, ",#SelectObjects=", #SelectObjects)
    end
    self.AxisActor.BoxComponent.bRenderVisibility = bShowBox
    
    self.AxisActor.TransfromComponentX.bRenderVisibility = bShowMove
    self.AxisActor.TransfromComponentY.bRenderVisibility = bShowMove
    self.AxisActor.TransfromComponentZ.bRenderVisibility = bShowMove
    
    self.AxisActor.RotationComponentX.bRenderVisibility = bShowRotation
    self.AxisActor.RotationComponentY.bRenderVisibility = bShowRotation
    self.AxisActor.RotationComponentZ.bRenderVisibility = bShowRotation

    _G.UGC.SelectInfo.SelectAxis = _G.UGC.AxisType.Axis_None
    _G.UGC.SelectInfo.NeedClearSelectAxis = false
end

function M:TrySelectAxis(MouseEvent)
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    if #SelectObjects == 0 then
        return false
    end
    if self.AxisActor == nil then
        return false
    end
    local Transfrom_Mode = _G.UGC.SelectInfo.Transfrom_Mode
    local OperatorType = _G.UGC.OperatorType
    if Transfrom_Mode == OperatorType.Tf_None then
        return false
    end

    local World = self:GetWorld()
    local ScreenPosition = UE.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标(这个是屏幕坐标噢，不是当前窗口的坐标）
    local ViewportPosition = UE.UUGCFunctionLibary.ScreenToWindow(ScreenPosition) -- 转换成当前的视图坐标(这个才是窗口的相对坐标)

    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(World, 0)
    -- K2_LineTraceComponent
    local AxisType = _G.UGC.AxisType
    
    local DefaultThickness = 2
    local AxisComponents = {}
    local AxisTypes = { AxisType.Axis_X, AxisType.Axis_Y, AxisType.Axis_Z }
    if Transfrom_Mode == OperatorType.Tf_Move then
        -- 平移
        AxisComponents[1] = self.AxisActor.TransfromComponentX
        AxisComponents[2] = self.AxisActor.TransfromComponentY
        AxisComponents[3] = self.AxisActor.TransfromComponentZ
    elseif Transfrom_Mode == OperatorType.Tf_Rotaion then
        -- 旋转
        AxisComponents[1] = self.AxisActor.RotationComponentX
        AxisComponents[2] = self.AxisActor.RotationComponentY
        AxisComponents[3] = self.AxisActor.RotationComponentZ
    elseif Transfrom_Mode == OperatorType.Tf_Scale then
        -- 缩放
        AxisComponents[1] = self.AxisActor.TransfromComponentX
        AxisComponents[2] = self.AxisActor.TransfromComponentY
        AxisComponents[3] = self.AxisActor.TransfromComponentZ
        DefaultThickness = 4
    else
    end
    
    for i = 1, #AxisComponents do
        local Component = AxisComponents[i]
        Component.Thickness = DefaultThickness
    end

    local bNeedClearAxis = _G.UGC.SelectInfo.NeedClearSelectAxis
    _G.UGC.SelectInfo.NeedClearSelectAxis = true
    local bFind = _G.UGC.SelectInfo.SelectAxis ~= _G.UGC.AxisType.Axis_None
    for i = 1, #AxisComponents do
        local Component = AxisComponents[i]
        local bSuc = Component:IsPick(localPlayerControler, ViewportPosition, 10)
        if bSuc then
            Component.Thickness = 10
            bNeedClearAxis = false
            _G.UGC.SelectInfo.NeedClearSelectAxis = false
            _G.UGC.SelectInfo.SelectAxis = AxisTypes[i]
            bFind = true
            break
        end
    end

    if bNeedClearAxis then
        _G.UGC.SelectInfo.SelectAxis = _G.UGC.AxisType.Axis_None
    else
        local Index = _G.UGC.SelectInfo.SelectAxis
        local Component = AxisComponents[Index]
        if Component then
            Component.Thickness = 10
        end
    end

    return bFind
end

-- 获得拾取的地面位置
function M:GetTerranPickPos(Origin, ScreenPosition)   
    local world = self:GetWorld()     
    local WorldPosition = UE.FVector()
    local WorldDirection = UE.FVector()
    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(world, 0)
    -- local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    UE.UGameplayStatics.DeprojectScreenToWorld(localPlayerControler, ScreenPosition, WorldPosition, WorldDirection)
    
    local TerrainPlane = UE.FPlane(Origin, UE.FVector(0, 0, 1))
    local NewObjPos = TerrainPlane:IntersectLine(WorldPosition, WorldDirection)

    return NewObjPos
end

-- 得到摄像机方向的拾取位置
function M:GetCameraPickPos(Origin, ScreenPosition)  
    local world = self:GetWorld()
    local WorldPosition = UE.FVector()
    local WorldDirection = UE.FVector()
    local localPlayerControler = _G.UGameplayStatics.GetPlayerController(world, 0)
    -- local ScreenPosition = UE4.UKismetInputLibrary.PointerEvent_GetScreenSpacePosition(MouseEvent)  -- 取屏幕坐标
    UE.UGameplayStatics.DeprojectScreenToWorld(localPlayerControler, ScreenPosition, WorldPosition, WorldDirection)        
    WorldDirection:Normalize()
    
    local CameraRatation = localPlayerControler:K2_GetActorRotation()
    local CameraDir = CameraRatation:ToVector()
    CameraDir:Normalize()

    -- 选取摄像机的朝向做平面的法向量
    local FacePlane = UE.FPlane(Origin, CameraDir)

    local NewObjPos = FacePlane:IntersectLine(WorldPosition, WorldDirection)

    return NewObjPos
end

-- 得到第一个选中对象的位置
function M:GetFirstSelectActorPosition()
    local FirstSelectObj = _G.UGC.SelectInfo.SelectObjects[1]    
    local LastObjPos = UE.FVector()
    if FirstSelectObj then
        LastObjPos = FirstSelectObj:K2_GetActorLocation()
    end
    return LastObjPos
end

-- 得到第一个选中对象的中心位置
function M:GetFirstSelectOrigin()
    local FirstSelectObj = _G.UGC.SelectInfo.SelectObjects[1]    
    local Origin = UE.FVector()
    local BoxExtent = UE.FVector()
    if FirstSelectObj then
        FirstSelectObj:GetActorBounds(true, Origin, BoxExtent, true)
    end
    return Origin
end

function M:GetWorld()
    return self.GameInstance:GetWorld()
end

function M:CreateAxisObject(Location)    
    local world = self:GetWorld()
    local ActorClass = UE.AAxisActor    
    local objName = "AxisActor"
    local initRotation = UE.FRotator(0, 0, 0)
    local transform = UE.FTransform(initRotation:ToQuat(), Location)
    local moduleName = nil
    self.AxisActor = world:SpawnActor(ActorClass, transform, UE.ESpawnActorCollisionHandlingMethod.AlwaysSpawn, nil, nil, moduleName, nil, nil, objName)    
end

function M:ShowAxisActor(selectActor)
    local Origin = UE.FVector()
    local BoxExtent = UE.FVector()
    selectActor:GetActorBounds(true, Origin, BoxExtent, true)
    if self.AxisActor == nil then
        self:CreateAxisObject(Origin)
    end
    self.AxisActor:SetActorHiddenInGame(false)
    local SweepHitResult = UE.FHitResult()
    self.AxisActor:K2_GetRootComponent():K2_SetRelativeLocation(Origin, false, SweepHitResult, false)
    self:RefreshAxisRadius()
end

function M:RefreshAxisRadius()
    local SelectObjects = _G.UGC.SelectInfo.SelectObjects
    for i = 1, #SelectObjects do
        local Object = SelectObjects[i]
        -- FBoxSphereBounds
        -- UMeshComponent
        -- local MeshComponent = Object:GetComponentByClass(UE.UMeshComponent)
        -- local Bounds = SelectObject.RootComponent:CalcLocalBounds()
    end
end

function M:HideAxisActor()
    if self.AxisActor then
        self.AxisActor:SetActorHiddenInGame(true)
    end    
end

function M:ShowDebugLine(StartPt, EndPt)
    if self.AxisActor then
        self.SplineComponent = self.AxisActor.DebugLineComponent
        local Points = self.SplineComponent.Points
        Points:Clear()
        Points:Add(StartPt)
        Points:Add(EndPt)

        self.SplineComponent.bRenderVisibility = true
    end
end

function M:HideDebugLine()
    if self.SplineComponent then
        self.SplineComponent.bRenderVisibility = false
    end    
end

return M