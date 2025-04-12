-- Player object
Player = {}

-- Constants
local PlayerConfig = {
    forces = { up = 3000, down = -3000, left = -3000, right = 3000 },
}

-- Define the speed threshold below which the player is considered idle for footsteps
local movement_speed_threshold = 10.0 -- Adjust as needed

-- Cheat mode state
local cheatModeEnabled = false
local godModeEnabled = false
local highDamageEnabled = false

-- SFX setting flag
local SFXSetFlag = false;

-- Function to initialize additional SFX on the entity (Optional: Keep if used for other sounds)
function Player:InitSFX(entity)
    -- SFX list (an array of audio file names)
    local extraSFX = {"WalkingNature.wav", "TakeDamageMeow2.wav", "PlayerDeathMeow2.wav", }
    -- Call the C++ binding function within this function context.
    SetAdditionalSFX(entity, extraSFX)
    SFXSetFlag = true
end

-- Player animation AND Footstep Trigger
function Player:Animate(entity, args)
    -- Get velocity and speed
    local velo = Velocity(entity)
    local speed = NetVelocity(entity)
    local is_moving_this_frame = (speed > movement_speed_threshold) -- Check if moving

    -- Call the C++ PlaySFX binding every frame.
    -- This function now handles the footstep timer and playback internally in C++.
    PlaySFX(entity, is_moving_this_frame) -- Pass true if moving, false if idle

    -- Determine animation based on speed and direction
    if not is_moving_this_frame then
        -- *** IDLE ANIMATION LOGIC ***
        local lastDirection = LastDirection(entity)
        if lastDirection == 0 then -- Idle facing right
            AnimationSet(entity, 0, 1, 5, 1)
            FlipX(entity, false)
        elseif lastDirection == 1 then -- Idle facing up-right
            AnimationSet(entity, 0, 2, 5, 2)
            FlipX(entity, false)
        elseif lastDirection == 2 then -- Idle facing up
            AnimationSet(entity, 0, 3, 5, 3)
            FlipX(entity, false)
        elseif lastDirection == 3 then -- Idle facing up-left
            AnimationSet(entity, 0, 2, 5, 2)
            FlipX(entity, true)
        elseif lastDirection == 4 then -- Idle facing down-right
            AnimationSet(entity, 0, 1, 5, 1) -- Changed from 0,5 to 0,1 to match right idle
            FlipX(entity, false)
        elseif lastDirection == 5 then -- Idle facing down
            AnimationSet(entity, 0, 0, 5, 0)
            FlipX(entity, false)
        elseif lastDirection == 6 then -- Idle facing down-left
            AnimationSet(entity, 0, 1, 5, 1) -- Changed from 0,5 to 0,1 to match left idle
            FlipX(entity, true)
        else -- Idle facing left (assuming lastDirection == 7 or default)
            AnimationSet(entity, 0, 1, 5, 1)
            FlipX(entity, true)
        end
        -- NOTE: The old PlaySFX(entity, false) call is removed from here.

    else
        -- *** MOVING ANIMATION LOGIC ***
        local angle = atan2(velo.y, velo.x)
        if angle >= -math.pi / 8 and angle < math.pi / 8 then -- Moving right
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, false)
            SetLastDirection(entity, 0)
        elseif angle >= math.pi / 8 and angle < 3 * math.pi / 8 then -- Moving up-right
            AnimationSet(entity, 0, 6, 9, 6)
            FlipX(entity, false)
            SetLastDirection(entity, 1)
        elseif angle >= 3 * math.pi / 8 and angle < 5 * math.pi / 8 then -- Moving up
            AnimationSet(entity, 0, 7, 9, 7)
            FlipX(entity, false)
            SetLastDirection(entity, 2)
        elseif angle >= 5 * math.pi / 8 and angle < 7 * math.pi / 8 then -- Moving up-left
            AnimationSet(entity, 0, 6, 9, 6)
            FlipX(entity, true)
            SetLastDirection(entity, 3)
        elseif angle >= -3 * math.pi / 8 and angle < -math.pi / 8 then -- Moving down-right
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, false)
            SetLastDirection(entity, 4)
        elseif angle >= -5 * math.pi / 8 and angle < -3 * math.pi / 8 then -- Moving down
            AnimationSet(entity, 0, 4, 9, 4)
            FlipX(entity, false)
            SetLastDirection(entity, 5)
        elseif angle >= -7 * math.pi / 8 and angle < -5 * math.pi / 8 then -- Moving down-left
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, true)
            SetLastDirection(entity, 6)
        else -- Moving left (angle >= 7 * math.pi / 8 or angle < -7 * math.pi / 8)
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, true)
            SetLastDirection(entity, 7)
        end
        -- NOTE: The old PlaySFX(entity, true) call is removed from here.
    end
end

-- Player Move function
function Player:Move(entity)
    -- Initialize direction variables
    local dx, dy = 0.0, 0.0

    -- Determine movement direction
    if IsKeyPressed(Key.W) then
        dy = dy + 1
    end

    if IsKeyPressed(Key.A) then
        dx = dx - 1
    end

    if IsKeyPressed(Key.S) then
        dy = dy - 1
    end

    if IsKeyPressed(Key.D) then
        dx = dx + 1
    end

    -- Normalize direction vector if moving diagonally
    local length = math.sqrt(dx * dx + dy * dy)
    if length > 0 then
        dx = dx / length
        dy = dy / length
    end

    -- Apply force in normalized direction
    local fx = dx * PlayerConfig.forces.right  -- Using PlayerConfig
    local fy = dy * PlayerConfig.forces.up     -- Using PlayerConfig

    ApplyForce(entity, fx, fy)
end


-- Player shoot function
function Player:Shoot(entity)
    local lastShotTime = GetLastShotTime()
    local shotCooldown = GetShotCooldown()
    local deltaTime = GetDeltaTime() -- Assuming GetDeltaTime exists and gives frame time

    -- Accumulate elapsed time
    lastShotTime = lastShotTime + deltaTime
    SetLastShotTime(lastShotTime)

    -- If not enough time has passed, prevent shooting
    if lastShotTime < shotCooldown then
        return
    end

    if IsMouseTriggered(Key.MOUSE_LEFT) then
        -- Reset cooldown timer
        SetLastShotTime(0.0)

        -- Get direction based on last direction (assuming mouse aim might be better, but using last direction for now)
        -- local direction = LastDirection(entity) -- Keep if using 8-way shooting based on movement
        
        -- Fire the bullet (assuming FireBullet handles direction calculation based on mouse or player facing)
        FireBullet(entity) -- Simplified call if FireBullet internally handles direction

        -- Handle shooting animation based on last direction (if needed)
        local direction = LastDirection(entity) -- Get direction for animation
        if direction == 0 then
            AnimationSet(entity, 0, 9, 2, 9)
            FlipX(entity, false)
        elseif direction == 1 then
            AnimationSet(entity, 0, 10, 2, 10)
            FlipX(entity, false)
        elseif direction == 2 then
            AnimationSet(entity, 0, 11, 2, 11)
            FlipX(entity, false)
        elseif direction == 3 then
            AnimationSet(entity, 0, 10, 2, 10)
            FlipX(entity, true)
        elseif direction == 4 then
            AnimationSet(entity, 0, 9, 2, 9) -- Down-right same as right? Check spritesheet
            FlipX(entity, false)
        elseif direction == 5 then
            AnimationSet(entity, 0, 8, 2, 8)
            FlipX(entity, false)
        elseif direction == 6 then
            AnimationSet(entity, 0, 9, 2, 9) -- Down-left same as left? Check spritesheet
            FlipX(entity, true)
        else -- direction 7 (Left)
            AnimationSet(entity, 0, 9, 2, 9)
            FlipX(entity, true)
        end

        -- Change state to Attack
        SetState(entity, "Attack")
    end
end

-- Player update function
function Player:update(args)
    local entity = args.entity

    -- Initialize additional SFX only once (Keep if needed)
    if not SFXSetFlag then
        Player:InitSFX(entity)
    end

    -- Handle attack animation state
    if State(entity) == "Attack" then
        if AnimationCompleted(entity) >= 1 then
            -- After attack animation finishes, return to idle or movement
            SetState(entity, "Idle")
        else
            -- While attacking, ensure footstep sounds stop by telling C++ we're not moving
            PlaySFX(entity, false)
            return -- Do not process movement/shooting/normal anims while attacking
        end
    end

    -- If not attacking, allow movement, animations (which triggers C++ footsteps), and shooting
    Player:Move(entity)
    Player:Animate(entity, args)
    Player:Shoot(entity)

    -- Handle death stuff
    if CheckDeath(entity) then
        -- Player is dying, ensure footsteps stop
        PlaySFX(entity, false)

        -- Change state to Death
        SetState(entity, "Death")
        AnimationSet(entity, 0, 12, 1, 12) -- Assuming death animation frames
        FlipX(entity, false)

        -- If animations have been completed
        if AnimationCompleted(entity) >= 1 then
            -- KillEntity(entity) -- Consider delaying this or handling via state machine / event
        end
    end
end

-- Test function (Keep as is or remove)
function Player:Test(args)
	local entity = args.entity
	local component = GetComponent(entity, "Transform::Transform")

	-- Check if the component is valid (not nil)
	if component ~= nil then
    	-- Access the component's properties, e.g., position, scale, rotation
    	cout(component.position.x)
    	cout(component.position.y)
	else
	    cout("Component 'Transform' not found for entity.")
	end
end

-- Return player object
return Player