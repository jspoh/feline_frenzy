------------------------------------------------------------------------------------
--file   enemy.lua
--brief  Enemy Lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

Patrol = {
    startPos = 0,           -- Starting position of the patrol
    endPos = 200,           -- End position of the patrol
    speed = 50.0,           -- Movement speed
    direction = 1           -- Current direction (1 = right, -1 = left)
}

-- Initialize function to set patrol range based on entity position
function Patrol:init(entity)
    local entityPos = getPosition(entity)
    self.startPos = entityPos.x - 100
    self.endPos = entityPos.x + 100
end

-- Update function to be called every frame for patrolling behavior
function Patrol:update(entity)
    local entityPos = getPosition(entity)
    
    -- Reverse direction if at the patrol bounds
    if entityPos.x >= self.endPos then
        self.direction = -1
    elseif entityPos.x <= self.startPos then
        self.direction = 1
    end
    
    -- Apply force to move the entity
    applyForce(entity, self.speed * self.direction, 0)
end

-- Return Patrol table so it can be accessed from C++
return Patrol