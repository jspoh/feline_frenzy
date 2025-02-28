function Clamp(val, low, high)
    if val < low then return low end
    if val > high then return high end
    return val
end

if MusicSettings == nil then
    MusicSettings = {}
end

function OnInit()
    local pos = GetPosition(entity)
    local scale = GetScale(entity)
    -- Assume the slider knob’s x–position gives the left bound,
    -- and the slider bar’s scale.x is stored in the knob’s Transform
    MusicSettings.left = pos.x
    MusicSettings.right = pos.x + scale.x
    MusicSettings.volume = 1.0  -- default volume (full volume)
end

function OnUpdate()
    -- If the slider knob is being dragged:
    if IsMousePressed(MOUSE_LEFT) and IsEntityHovered(entity) then
        local mousePos = WindowMousePos()
        local newX = Clamp(mousePos.x, MusicSettings.left, MusicSettings.right)
        SetPosition(entity, newX, GetPosition(entity).y)
        local rel = (newX - MusicSettings.left) / (MusicSettings.right - MusicSettings.left)
        MusicSettings.volume = rel
        SetGlobalVolume("Menu_BGM", rel)
    else
        -- Otherwise, update the knob’s x–position to match the current volume.
        local vol = MusicSettings.volume or 1.0
        local targetX = MusicSettings.left + vol * (MusicSettings.right - MusicSettings.left)
        SetPosition(entity, targetX, GetPosition(entity).y)
    end
end