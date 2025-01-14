------------------------------------------------------------------------------------
--file   player.lua
--brief  Player lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

-- Player object
Player = {}

-- Constants
local PlayerConfig = {
    forces = { up = 1000, down = -1000, left = -1000, right = 1000 },
}

-- Cheat mode state
local cheatModeEnabled = false
local godModeEnabled = false
local highDamageEnabled = false

-- Player animation
function Player:Animate(entity, args)
    -- Get Last Direction
    local lastDirection = LastDirection(entity)
	
    -- Get velocity
    local velo = Velocity(entity)
    
    -- Get speed
    local speed = NetVelocity(entity)
    
    if speed < 1.0 then
        -- Idle Animation: Use last direction for idle
        if lastDirection == 0 then
            -- Idle facing right
            AnimationStart(entity, 0, 1)
            AnimationEnd(entity, 5, 1)
            FlipX(entity, false)
            PlaySFX(entity, false)
        elseif lastDirection == 1 then
            -- Idle facing up-right
            AnimationStart(entity, 0, 2)
            AnimationEnd(entity, 5, 2)
            FlipX(entity, false)
            PlaySFX(entity, false)
        elseif lastDirection == 2 then
            -- Idle facing up
            AnimationStart(entity, 0, 3)
            AnimationEnd(entity, 5, 3)
            FlipX(entity, false)
            PlaySFX(entity, false)
        elseif lastDirection == 3 then
            -- Idle facing up-left
            AnimationStart(entity, 0, 2)
            AnimationEnd(entity, 5, 2)
            FlipX(entity, true)
            PlaySFX(entity, false)
        elseif lastDirection == 4 then
            -- Idle facing down-right
            AnimationStart(entity, 0, 1)
            AnimationEnd(entity, 5, 1)
            FlipX(entity, false)
            PlaySFX(entity, false)
        elseif lastDirection == 5 then
            -- Idle facing down
            AnimationStart(entity, 0, 0)
            AnimationEnd(entity, 5, 0)
            FlipX(entity, false)
            PlaySFX(entity, false)
        elseif lastDirection == 6 then
            -- Idle facing down-left
            AnimationStart(entity, 0, 1)
            AnimationEnd(entity, 5, 1)
            FlipX(entity, true)
            PlaySFX(entity, true)
        else
            -- Idle facing left
            AnimationStart(entity, 0, 1)
            AnimationEnd(entity, 5, 1)
            FlipX(entity, true)
            PlaySFX(entity, true)
        end
    else
        -- Calculate angle of movement (in radians)
        local angle = atan2(velo.y, velo.x)
        
        -- Map angle to 8 directions
        if angle >= -math.pi / 8 and angle < math.pi / 8 then
            -- Moving right
            AnimationStart(entity, 0, 5)
            AnimationEnd(entity, 9, 5)
            FlipX(entity, false)
            SetLastDirection(entity, 0)
        elseif angle >= math.pi / 8 and angle < 3 * math.pi / 8 then
            -- Moving up-right
            AnimationStart(entity, 0, 6)
            AnimationEnd(entity, 9, 6)
            FlipX(entity, false)
            SetLastDirection(entity, 1)
        elseif angle >= 3 * math.pi / 8 and angle < 5 * math.pi / 8 then
            -- Moving up
            AnimationStart(entity, 0, 7)
            AnimationEnd(entity, 9, 7)
            FlipX(entity, false)
            SetLastDirection(entity, 2)
        elseif angle >= 5 * math.pi / 8 and angle < 7 * math.pi / 8 then
            -- Moving up-left
            AnimationStart(entity, 0, 6)
            AnimationEnd(entity, 9, 6)
            FlipX(entity, true)
            SetLastDirection(entity, 3)
        elseif angle >= -3 * math.pi / 8 and angle < -math.pi / 8 then
            -- Moving down-right
            AnimationStart(entity, 0, 5)
            AnimationEnd(entity, 9, 5)
            FlipX(entity, false)
            SetLastDirection(entity, 4)
        elseif angle >= -5 * math.pi / 8 and angle < -3 * math.pi / 8 then
            -- Moving down
            AnimationStart(entity, 0, 4)
            AnimationEnd(entity, 9, 4)
            FlipX(entity, false)
            SetLastDirection(entity, 5)
        elseif angle >= -7 * math.pi / 8 and angle < -5 * math.pi / 8 then
            -- Moving down-left
            AnimationStart(entity, 0, 5)
            AnimationEnd(entity, 9, 5)
            FlipX(entity, true)
            SetLastDirection(entity, 6)
        else
            -- Moving left
            AnimationStart(entity, 0, 5)
            AnimationEnd(entity, 9, 5)
            FlipX(entity, true)
            SetLastDirection(entity, 7)
        end

        PlaySFX(entity, true)
    end
end

-- Player movement
function Player:Move(entity)

    -- Initialize force variables
    local fx, fy = 0.0, 0.0

    -- Accumulate forces based on key presses
    if IsKeyPressed(Key.W) then
        fy = fy + PlayerConfig.forces.up -- Add upward force
    end

    if IsKeyPressed(Key.A) then
        fx = fx + PlayerConfig.forces.left -- Add leftward force
    end

    if IsKeyPressed(Key.S) then
        fy = fy + PlayerConfig.forces.down -- Add downward force
    end

    if IsKeyPressed(Key.D) then
        fx = fx + PlayerConfig.forces.right -- Add rightward force
    end

    -- Apply the combined force to the entity
    ApplyForce(entity, fx, fy)

end

-- Player shoot function
function Player:Shoot(entity)

    if IsMouseTriggered(Key.MOUSE_LEFT) then
        -- Check last direction before firing the bullet
        local direction = lastDirection

        -- Fire the bullet and pass the direction as an argument
        FireBullet(entity, direction)
        
        -- if direction == 0 then
        --     -- Shooting to the right
        --     AnimationStart(entity, 0, 9)
        --     AnimationEnd(entity, 2, 9)
        --     FlipX(entity, false)
        -- elseif direction == 1 then
        --     -- Shooting up-right
        --     AnimationStart(entity, 0, 10)
        --     AnimationEnd(entity, 2, 10)
        --     FlipX(entity, false)
        -- elseif direction == 2 then
        --     -- Shooting up
        --     AnimationStart(entity, 0, 11)
        --     AnimationEnd(entity, 2, 11)
        --     FlipX(entity, false)
        -- elseif direction == 3 then
        --     -- Shooting up-left
        --     AnimationStart(entity, 0, 10)
        --     AnimationEnd(entity, 2, 10)
        --     FlipX(entity, true)
        -- elseif direction == 4 then
        --     -- Shooting down-right
        --     AnimationStart(entity, 0, 9)
        --     AnimationEnd(entity, 2, 9)
        --     FlipX(entity, false)
        -- elseif direction == 5 then
        --     -- Shooting down
        --     AnimationStart(entity, 0, 8)
        --     AnimationEnd(entity, 2, 8)
        --     FlipX(entity, false)
        -- elseif direction == 6 then
        --     -- Shooting down-left
        --     AnimationStart(entity, 0, 9)
        --     AnimationEnd(entity, 2, 9)
        --     FlipX(entity, true)
        -- else
        --     -- Shooting left
        --     AnimationStart(entity, 0, 9)
        --     AnimationEnd(entity, 2, 9)
        --     FlipX(entity, true)
        -- end
    end

end

-- Player update function
function Player:update(args)

    -- Cheat mode enable
    if IsKeyTriggered(Key.KEY_0) and IsKeyTriggered(Key.KEY_9) then
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
        if IsKeyTriggered(Key.KEY_1) then
            -- Get mouse position
            local mousePos = WorldMousePos()

            -- Set player position to the mouse position
            SetPosition(entity, mousePos.x, mousePos.y)

            -- Log the new position
            cout("Teleported player to: X = " .. mousePos.x .. ", Y = " .. mousePos.y)
        end

        -- God Mode toggle
        if IsKeyTriggered(Key.KEY_2) then
            godModeEnabled = not godModeEnabled
            SetGodMode(entity, godModeEnabled)
        end

        -- High Damage toggle
        if IsKeyTriggered(Key.KEY_3) then
            highDamageEnabled = not highDamageEnabled
            SetHighDamage(entity, highDamageEnabled)
        end

    end

    Player:Animate(args.entity, args)

    Player:Move(args.entity)

    Player:Shoot(args.entity)

end

-- Return player object
return Player
