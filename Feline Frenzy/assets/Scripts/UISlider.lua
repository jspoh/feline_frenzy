-- UISlider.lua
-- This Lua script handles the UI slider functionality for both BGM and SFX volume.
-- It is intended to be attached to the slider arrow buttons.

local UISlider = {}

-- BGM slider functions
function UISlider:ArrowLeft()
    SliderArrowLeft()  -- Bound in sLuaGameBinds.cpp for BGM
end

function UISlider:ArrowRight()
    SliderArrowRight()  -- Bound in sLuaGameBinds.cpp for BGM
end

-- SFX slider functions
function UISlider:ArrowLeftSfx()
    SfxSliderArrowLeft()  -- Bound in sLuaGameBinds.cpp for SFX
end

function UISlider:ArrowRightSfx()
    SfxSliderArrowRight()  -- Bound in sLuaGameBinds.cpp for SFX
end

-- Optional: Called when the slider itself is dragged.
-- 'relativePos' should be a value between 0.0 (far left) and 1.0 (far right).
function UISlider:UpdateVolume(relativePos)
    local vol = math.min(math.max(relativePos, 0.0), 1.0)
    -- Additional logic for real-time update can be added here if needed.
end

return UISlider
