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

-- Initialize force variables
local fx, fy = 0.0, 0.0

-- Accumulate forces based on key presses
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

-- Apply the combined force to the entity
ApplyForce(entity, fx, fy)

end

--Player shoot function
function Player:Shoot(entity)

if IsMouseTriggered(Key.MOUSE_LEFT) then
cout("SHOOTING NIGGERS")
end

end

--Player update function
function Player:update(args)

Player:Animate(args.entity, args)

Player:Move(args.entity)

Player:Shoot(args.entity)

end

--Return player object
return Player
