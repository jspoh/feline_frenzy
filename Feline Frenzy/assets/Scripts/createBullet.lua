-- Define a module table
local Bullet = {}

-- Function to create a bullet from a prefab
function Bullet.createBullet(layer, prefab_path, shooter_x, shooter_y, offset)

    -- Call C++ function to create a bullet from the prefab
    -- cout("CALLING SHOOT BULLET");
    -- cout("Shooter X: " .. tostring(shooter_x) .. ", Shooter Y: " .. tostring(shooter_y))

    shootBullet(layer, prefab_path, "bullet_" .. prefab_path, shooter_x, shooter_y, offset)

    -- cout("Creating bullet from prefab: " .. prefab_path)
    -- Create a bullet from the prefab
    -- SHIT HOW DID THIS BECOME CPP SPAWNING
    -- local bullet_id = createBulletFromPrefab(layer, prefab_path)

    -- Log creation success
    -- cout("Bullet created, ID: " .. bullet_id)
    
    return bullet_id
end

-- Return the module table
return Bullet

