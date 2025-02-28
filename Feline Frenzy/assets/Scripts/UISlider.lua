-- UISlider.lua
function OnInit()
    -- 'entity' is assumed to be a global variable referring to this entity’s ID.
    RegisterUIEntity("BGMSlider", entity)
end

function OnUpdate()
    -- No additional per–frame Lua logic is needed;
    -- the C++ update() will adjust its position.
end
