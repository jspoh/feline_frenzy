------------------------------------------------------------------------------------
--file   StateMachine.lua
--brief  Lua manager
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

--StateMachine object
StateMachine = {
    currentState = "Idle",   -- Starting state
    startPosition = 0,       -- Initial position
    targetPosition = 100,    -- Target position for movement
    currentPosition = 0      -- Track current position
}

-- Transition to the next state based on the current state
function StateMachine:transition(entity)
    if self.currentState == "Idle" then
        self:enter(entity)
    elseif self.currentState == "Moving" then
        self:update(entity)
    elseif self.currentState == "Return" then
        self:exit(entity)
    end
end

--StateMachine enter function
function StateMachine:enter(entity)
    self.currentPosition = self.startPosition
    self.currentState = "Moving"
end

--StateMachine update function
function StateMachine:update(entity)
    print("Moving entity to target position")

    -- Increment position (example movement logic)
    self.currentPosition = self.currentPosition + 1

    --Check if reached target position
    if self.currentPosition >= self.targetPosition then
        print("Reached target position")
        self.currentState = "Return"  -- Transition to Return state
    end
end

--StateMachine exit function
function StateMachine:exit(entity)
    --Decrement position to move back to start
    self.currentPosition = self.currentPosition - 1

    --Check if reached start position
    if self.currentPosition <= self.startPosition then
        self.currentState = "Idle"  -- Transition to Idle state
    end
end

--Return StateMachine object
return StateMachine