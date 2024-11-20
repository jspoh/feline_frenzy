-- Define a module table
local Bullet = {}

-- Function to create a bullet from a prefab
function Bullet.createBullet(layer, prefab_path)
    -- Call C++ function to create a bullet from the prefab
    createEntityFromPrefab(layer, prefab_path, "bullet_" .. prefab_path)

    -- Create a bullet from the prefab
    local bullet_id = createBulletFromPrefab(layer, prefab_path)

    -- Log creation success
    -- cout("Bullet created, ID: " .. bullet_id)
    
    return bullet_id
end

-- Return the module table
return Bullet

