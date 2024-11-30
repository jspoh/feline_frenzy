------------------------------------------------------------------------------------
--file   ChangeScene.lua
--brief  Changin scene lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

Scenes = {}

--Change Scene
function Scenes:Change(args)
ChangeScene(args.scene)
end

--Restart Scene 
function Scenes:Restart()
RestartScene()
end

--Previous Scene 
function Scenes:Previous()
PreviousScene()
end

--Close Scene 
function Scenes:Close()
CloseScene()
end

return Scenes