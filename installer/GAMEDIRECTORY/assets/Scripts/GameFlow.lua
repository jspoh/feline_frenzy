------------------------------------------------------------------------------------
--file   GameFlow.lua
--brief  Game Flow lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

GameFlow = {}

-- State Variables
GameFlow.isGameOver = false
GameFlow.waveState = {
    numEnemyToSpawn = 15,
    numEnemySpawned = 0,
    numEnemyAlive = 0,
    spawnTimeout = 0,
}

-- Handle Game End
function GameFlow:endGame()
    self.isGameOver = true
end

-- Spawn Enemy Logic
function GameFlow:spawnEnemy()



    -- Update wave state
    self.waveState.numEnemySpawned = self.waveState.numEnemySpawned + 1
    self.waveState.numEnemyAlive = self.waveState.numEnemyAlive + 1
end

-- Update Logic (called every frame)
function GameFlow:update()
    -- Check if the player is dead
    if player.health <= 0 and not self.isGameOver then
        self:endGame()
        return
    end

    -- Handle enemy spawning
    if self.waveState.numEnemySpawned < self.waveState.numEnemyToSpawn then
        self:spawnEnemy()
    end
end

return GameFlow