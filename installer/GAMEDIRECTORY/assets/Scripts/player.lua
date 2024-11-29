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

--Cheat mode state
local cheatModeEnabled = false
local godModeEnabled = false
local highDamageEnabled = false

--Player update function
function Player:update(entity)
    
    -- Cheat mode enable
    if isKeyTriggered(Key["0"]) and isKeyTriggered(Key["9"]) then
        cheatModeEnabled = not cheatModeEnabled
        if cheatModeEnabled then
            cout("Cheat mode enabled")
        end
        if not cheatModeEnabled then
            cout("Cheat mode disabled")
        end
    end

    if cheatModeEnabled then
        -- Teleport to cursor cheat
        if isKeyTriggered(Key["1"]) then
            -- Get mouse position
            local mousePos = getMousePos()

            -- Set player position to the mouse position
            setPosition(entity, mousePos.x, mousePos.y)

            -- Log the new position
            cout("Teleported player to: X = " .. mousePos.x .. ", Y = " .. mousePos.y)
        end

        -- God Mode toggle
        if isKeyTriggered(Key["2"]) then
            godModeEnabled = not godModeEnabled
            setGodMode(entity, godModeEnabled)
        end

        -- High Damage toggle
        if isKeyTriggered(Key["3"]) then
            highDamageEnabled = not highDamageEnabled
            setHighDamage(entity, highDamageEnabled)
        end
    end

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