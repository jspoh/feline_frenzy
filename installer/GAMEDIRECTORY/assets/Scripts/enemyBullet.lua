-- Define a module table
local Bullet = {}

-- Function to create a bullet from a prefab
function Bullet.enemyBullet(layer, prefab_path, enemy_x, enemy_y, player_x, player_y, offset)

    -- Call C++ function to create a bullet from the prefab
    enemyBullet(layer, prefab_path, "bullet_" .. prefab_path, enemy_x, enemy_y, player_x, player_y, offset)
    -- cout("Creating bullet from prefab: " .. prefab_path)

    -- Log creation success
    -- cout("Bullet created, ID: " .. bullet_id)
    
    return bullet_id
end

-- Return the module table
return Bullet
