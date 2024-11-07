-- Define the player table to encapsulate all player-specific logic
Player = {}

-- Update function to be called every frame
function Player:update(entity)
    --Reset force
    applyXForce(entity, 0.0)
    applyYForce(entity, 0.0)

    if isKeyPressed(Key.W) then
        applyYForce(entity, 100.0)  -- Upward force
    end

    if isKeyPressed(Key.S) then
        applyYForce(entity, -100.0) -- Downward force
    end

    if isKeyPressed(Key.A) then
        applyXForce(entity, -100.0) -- Left force
    end

    if isKeyPressed(Key.D) then
        applyXForce(entity, 100.0)  -- Right force
    end
end

-- Return the player table to be accessible from C++
return Player