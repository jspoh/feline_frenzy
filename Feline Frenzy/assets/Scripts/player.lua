------------------------------------------------------------------------------------
--file   player.lua
--brief  Player lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

--Player object
Player = {}

--Player update function
function Player:update(entity)

    --Reset force
    applyXForce(entity, 0.0)
    applyYForce(entity, 0.0)

    --Upward force
    if isKeyPressed(Key.W) then
        applyYForce(entity, 100.0)
    end

    --Downward force
    if isKeyPressed(Key.S) then
        applyYForce(entity, -100.0)
    end

    --Left force
    if isKeyPressed(Key.A) then
        applyXForce(entity, -100.0)
    end

    --Right force
    if isKeyPressed(Key.D) then
        applyXForce(entity, 100.0)
    end
end

--Return player object
return Player