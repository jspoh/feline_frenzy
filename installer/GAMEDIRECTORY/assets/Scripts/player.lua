-- Player object
Player = {}

-- Constants
local PlayerConfig = {
    forces = { up = 3000, down = -3000, left = -3000, right = 3000 },
}

-- Cheat mode state
local cheatModeEnabled = false
local godModeEnabled = false
local highDamageEnabled = false

-- SFX setting flag
local SFXSetFlag = false;

-- Function to initialize additional SFX on the entity
function Player:InitSFX(entity)
    -- SFX list (an array of audio file names)
    local extraSFX = {"WalkingNature.wav", "TakeDamageMeow2.wav", "PlayerDeathMeow2.wav", }
    -- Call the C++ binding function within this function context.
    SetAdditionalSFX(entity, extraSFX)
    SFXSetFlag = true
end

-- Player animation
function Player:Animate(entity, args)
    -- Get Last Direction
    local lastDirection = LastDirection(entity)
    
    -- Get velocity
    local velo = Velocity(entity)
    
    -- Get speed
    local speed = NetVelocity(entity)
    
    if speed < 100 then
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
            PlaySFX(entity, false)
        else
            -- Idle facing left
            AnimationStart(entity, 0, 1)
            AnimationEnd(entity, 5, 1)
            FlipX(entity, true)
            PlaySFX(entity, false)
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
        fy = fy + PlayerConfig.forces.up
    end

    if IsKeyPressed(Key.A) then
        fx = fx + PlayerConfig.forces.left
    end

    if IsKeyPressed(Key.S) then
        fy = fy + PlayerConfig.forces.down
    end

    if IsKeyPressed(Key.D) then
        fx = fx + PlayerConfig.forces.right
    end

    -- Apply the combined force to the entity
    ApplyForce(entity, fx, fy)
end

-- Player shoot function
function Player:Shoot(entity)
    if IsMouseTriggered(Key.MOUSE_LEFT) then
        -- Get direction based on last direction
        local direction = LastDirection(entity)

        -- Fire the bullet
        FireBullet(entity, direction)

        -- Handle shooting animation
        if direction == 0 then
            AnimationStart(entity, 0, 9)
            AnimationEnd(entity, 2, 9)
            FlipX(entity, false)    
        elseif direction == 1 then
            AnimationStart(entity, 0, 10)
            AnimationEnd(entity, 2, 10)
            FlipX(entity, false)
        elseif direction == 2 then
            AnimationStart(entity, 0, 11)
            AnimationEnd(entity, 2, 11)
            FlipX(entity, false)
        elseif direction == 3 then
            AnimationStart(entity, 0, 10)
            AnimationEnd(entity, 2, 10)
            FlipX(entity, true)
        elseif direction == 4 then
            AnimationStart(entity, 0, 9)
            AnimationEnd(entity, 2, 9)
            FlipX(entity, false)
        elseif direction == 5 then
            AnimationStart(entity, 0, 8)
            AnimationEnd(entity, 2, 8)
            FlipX(entity, false)
        elseif direction == 6 then
            AnimationStart(entity, 0, 9)
            AnimationEnd(entity, 2, 9)
            FlipX(entity, true)
        else
            AnimationStart(entity, 0, 9)
            AnimationEnd(entity, 2, 9)
            FlipX(entity, true)
        end

        -- Change state to Attack
        SetState(entity, "Attack")
    end
end

-- Player update function
function Player:update(args)
    local entity = args.entity

    -- Initialize additional SFX only once
    if not SFXSetFlag then
        Player:InitSFX(entity)
    end

    -- Handle attack animation state
    if State(entity) == "Attack" then
        if AnimationCompleted(entity) >= 1 then
            -- After attack animation finishes, return to idle or movement
            SetState(entity, "Idle")
        else
            -- While attacking, do not process movement or animations
            return
        end
    end

    -- If not attacking, allow movement and animations
    Player:Move(entity)
    Player:Animate(entity, args)
    Player:Shoot(entity)

    -- Handle death stuff
    CheckDeath(entity)
    -- Change state to Death
    SetState(entity, "Death")
    AnimationStart(entity, 0, 12)
    AnimationEnd(entity, 1, 12)
    FlipX(entity, false)
    if AnimationCompleted(entity) >= 1 then
        KillEntity(entity)
    end
end

-- Return player object
return Player
