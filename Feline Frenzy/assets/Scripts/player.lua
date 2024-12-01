------------------------------------------------------------------------------------
--file   player.lua
--brief  Player lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

--Player object
Player = {}

--Cheat mode state
local cheatModeEnabled = false
local godModeEnabled = false
local highDamageEnabled = false

--Player animation
function Player:Animate(entity, args)

--Get velocity
local velo = Velocity(entity)

--Get speed
local speed = NetVelocity(entity)

if speed < 1.0 then
    -- Idle Animation
    AnimationStart(entity, 0, 0)
    AnimationEnd(entity, 5, 0)
    FlipX(entity, false)
	PlaySFX(entity, false)
else
    -- Determine horizontal or vertical priority
    if math.abs(velo.x) > math.abs(velo.y) then
        -- Horizontal movement
        if velo.x > 0 then
            -- Moving right
            AnimationStart(entity, 0, 5)
            AnimationEnd(entity, 9, 5)
            FlipX(entity, false)
			PlaySFX(entity, true)
        else
            -- Moving left
            AnimationStart(entity, 0, 5)
            AnimationEnd(entity, 9, 5)
            FlipX(entity, true)
			PlaySFX(entity,true)
        end
    else
        -- Vertical movement
        if velo.y > 0 then
            -- Moving up
            AnimationStart(entity, 0, 7)
            AnimationEnd(entity, 9, 7)
            FlipX(entity, false)
			PlaySFX(entity, true)
        else
            -- Moving down
            AnimationStart(entity, 0, 4)
            AnimationEnd(entity, 9, 4)
            FlipX(entity, false)
			PlaySFX(entity, true)
        end
    end
end

end


--Player movement
function Player:Move(entity)

--Initialize force variables
local fx, fy = 0.0, 0.0

--Accumulate forces based on key presses
if IsKeyPressed(Key.W) then
	fy = fy + 100.0 -- Add upward force
end

if IsKeyPressed(Key.A) then
   fx = fx - 100.0 -- Add leftward force
end

if IsKeyPressed(Key.S) then
   fy = fy - 100.0 -- Add downward force
end

if IsKeyPressed(Key.D) then
   fx = fx + 100.0 -- Add rightward force
end

--Apply the combined force to the entity
ApplyForce(entity, fx, fy)

end

--Player shoot function
function Player:Shoot(entity)

if IsMouseTriggered(Key.MOUSE_LEFT) then
    FireBullet(entity)
end

end

--Player update function
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

--Return player object
return Player
