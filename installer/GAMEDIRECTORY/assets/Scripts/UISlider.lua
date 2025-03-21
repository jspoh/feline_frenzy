-- UISlider.lua
-- This Lua script handles the UI slider for adjusting BGM and SFX volumes.
local UISlider = {}

-- Called when the BGM left arrow is pressed.
function UISlider:ArrowLeft()
    SliderArrowLeft()
end

-- Called when the BGM right arrow is pressed.
function UISlider:ArrowRight()
    SliderArrowRight()
end

-- Called when the SFX left arrow is pressed.
function UISlider:SfxArrowLeft()
    SfxSliderArrowLeft()
end

-- Called when the SFX right arrow is pressed.
function UISlider:SfxArrowRight()
    SfxSliderArrowRight()
end

-- Optional: Called when the slider itself is dragged.
-- 'relativePos' should be a value between 0.0 (far left) and 1.0 (far right).
function UISlider:UpdateVolume(relativePos)
    local vol = math.min(math.max(relativePos, 0.0), 1.0)
    -- Here you might call a bound function to update the volume directly.
    -- For example: SetGlobalBGMVolume(vol)
end

return UISlider
