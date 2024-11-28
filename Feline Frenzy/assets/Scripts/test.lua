------------------------------------------------------------------------------------
--file   player.lua
--brief  Player lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

--Player object
Player = {}

--Player update function
function Player:update()
local v = Vector2f(0.0f, 100.0f)
cout(v.y)
end

--Return player object
return Player