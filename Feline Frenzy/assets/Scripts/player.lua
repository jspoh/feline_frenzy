-- Schema configs (for editor purposes)
schema = {
    forces_up = 3000,
    forces_down = -3000,
    forces_left = -3000,
    forces_right = 3000
}

-- Player "class"
local Player = {}
Player.__index = Player

-- Constructor
function Player:new(config)
    local self = setmetatable({}, Player)
    self.config = config or {}
    self.sfx_set = false
    return self
end

-- SFX Initialization
function Player:InitSFX(entity)
    local extraSFX = {"WalkingNature.wav", "TakeDamageMeow2.wav", "PlayerDeathMeow2.wav"}
    SetAdditionalSFX(entity, extraSFX)
    self.sfx_set = true
end

-- Player Animation
function Player:Animate(entity)
    local lastDirection = LastDirection(entity)
    local velo = Velocity(entity)
    local speed = NetVelocity(entity)

    if speed < 100 then
        -- Idle animation
        if lastDirection == 0 then
            AnimationSet(entity, 0, 1, 5, 1)
            FlipX(entity, false)
        elseif lastDirection == 1 then
            AnimationSet(entity, 0, 2, 5, 2)
            FlipX(entity, false)
        elseif lastDirection == 2 then
            AnimationSet(entity, 0, 3, 5, 3)
            FlipX(entity, false)
        elseif lastDirection == 3 then
            AnimationSet(entity, 0, 2, 5, 2)
            FlipX(entity, true)
        elseif lastDirection == 4 then
            AnimationSet(entity, 0, 1, 5, 1)
            FlipX(entity, false)
        elseif lastDirection == 5 then
            AnimationSet(entity, 0, 0, 5, 0)
            FlipX(entity, false)
        elseif lastDirection == 6 then
            AnimationSet(entity, 0, 1, 5, 1)
            FlipX(entity, true)
        else
            AnimationSet(entity, 0, 1, 5, 1)
            FlipX(entity, true)
        end
        PlaySFX(entity, false)
    else
        -- Moving animation
        local angle = atan2(velo.y, velo.x)
        if angle >= -math.pi / 8 and angle < math.pi / 8 then
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, false)
            SetLastDirection(entity, 0)
        elseif angle >= math.pi / 8 and angle < 3 * math.pi / 8 then
            AnimationSet(entity, 0, 6, 9, 6)
            FlipX(entity, false)
            SetLastDirection(entity, 1)
        elseif angle >= 3 * math.pi / 8 and angle < 5 * math.pi / 8 then
            AnimationSet(entity, 0, 7, 9, 7)
            FlipX(entity, false)
            SetLastDirection(entity, 2)
        elseif angle >= 5 * math.pi / 8 and angle < 7 * math.pi / 8 then
            AnimationSet(entity, 0, 6, 9, 6)
            FlipX(entity, true)
            SetLastDirection(entity, 3)
        elseif angle >= -3 * math.pi / 8 and angle < -math.pi / 8 then
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, false)
            SetLastDirection(entity, 4)
        elseif angle >= -5 * math.pi / 8 and angle < -3 * math.pi / 8 then
            AnimationSet(entity, 0, 4, 9, 4)
            FlipX(entity, false)
            SetLastDirection(entity, 5)
        elseif angle >= -7 * math.pi / 8 and angle < -5 * math.pi / 8 then
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, true)
            SetLastDirection(entity, 6)
        else
            AnimationSet(entity, 0, 5, 9, 5)
            FlipX(entity, true)
            SetLastDirection(entity, 7)
        end
        PlaySFX(entity, true)
    end
end

-- Player Movement
function Player:Move(entity)
    local dx, dy = 0.0, 0.0

    if IsKeyPressed(Key.W) then dy = dy + 1 end
    if IsKeyPressed(Key.A) then dx = dx - 1 end
    if IsKeyPressed(Key.S) then dy = dy - 1 end
    if IsKeyPressed(Key.D) then dx = dx + 1 end

    local length = math.sqrt(dx * dx + dy * dy)
    if length > 0 then
        dx = dx / length
        dy = dy / length
    end

    local fx = dx * self.config.forces_right
    local fy = dy * self.config.forces_up

    ApplyForce(entity, fx, fy)
end

-- Player Shooting
function Player:Shoot(entity)
    local lastShotTime = GetLastShotTime()
    local shotCooldown = GetShotCooldown()
    local deltaTime = GetDeltaTime()

    lastShotTime = lastShotTime + deltaTime
    SetLastShotTime(lastShotTime)

    if lastShotTime < shotCooldown then
        return
    end

    if IsMouseTriggered(Key.MOUSE_LEFT) then
        SetLastShotTime(0.0)

        local direction = LastDirection(entity)
        FireBullet(entity, direction)

        -- Shooting animation
        local animIndex = {
            [0] = 9, [1] = 10, [2] = 11, [3] = 10,
            [4] = 9, [5] = 8,  [6] = 9,  [7] = 9
        }
        AnimationSet(entity, 0, animIndex[direction], 2, animIndex[direction])
        FlipX(entity, direction == 3 or direction == 6 or direction == 7)
        SetState(entity, "Attack")
    end
end

-- Update function
function Player:update(entity)

    -- Set Extra SFX
    if not self.sfx_set then
        self:InitSFX(entity)
    end

    if State(entity) == "Attack" then
        if AnimationCompleted(entity) >= 1 then
            SetState(entity, "Idle")
        else
            return
        end
    end

    self:Move(entity)
    self:Animate(entity)
    self:Shoot(entity)

    if CheckDeath(entity) then
        SetState(entity, "Death")
        AnimationSet(entity, 0, 12, 1, 12)
        FlipX(entity, false)

        if AnimationCompleted(entity) >= 1 then
            KillEntity(entity)
        end
    end
end

-- Factory Function (for C++)
return function(config)
    local player = Player:new(config)
    return {
        update = function(entity)
            player:update(entity)
        end
    }
end
