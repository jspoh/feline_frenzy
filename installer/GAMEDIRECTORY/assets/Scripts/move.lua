------------------------------------------------------------------------------------
--file   enemy.lua
--brief  Enemy Lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

--Move object
Move = {}

--Move update function
function Move:update(entity, dir)

    --Upward force
    if dir == 0 then
        applyYForce(entity, 100.0)
    end

    --Downward force
    if dir == 1 then
        applyYForce(entity, -100.0)
    end

    --Left force
    if dir == 2 then
        applyXForce(entity, -100.0)
    end

    --Right force
    if dir == 3 then
        applyXForce(entity, 100.0)
    end
end

--Return Move object
return Move