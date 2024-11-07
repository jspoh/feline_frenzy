-- Define the player table to encapsulate all player-specific logic
Player = {}

-- Update function to be called every frame
function Player:update(entity)
cout(entity)
    -- Input handling with your defined key codes
    if isKeyPressed(Key.W) then
        applyForce(entity, 0.0, 100.0)  -- Example upward force
    end
    if isKeyPressed(Key.S) then
        applyForce(entity, 0.0, -100.0) -- Example downward force
    end
    if isKeyPressed(Key.A) then
        applyForce(entity, -100.0, 0.0) -- Example leftward force
    end
    if isKeyPressed(Key.D) then
        applyForce(entity, 100.0, 0.0)  -- Example rightward force
    end
end

-- Return the player table to be accessible from C++
return Player