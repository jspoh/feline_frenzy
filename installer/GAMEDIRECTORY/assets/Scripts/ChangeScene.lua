------------------------------------------------------------------------------------
--file   ChangeScene.lua
--brief  Changin scene lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

Scenes = {}

-- A simple blocking wait function, note this is a busy-wait loop and will block execution.
local function wait(seconds)
    local start = os.clock()
    while os.clock() - start < seconds do
        -- TODO should change to NOT use busy-wait if possible
    end
end

--Change Scene
function Scenes:Change(args)
--wait(1)  -- Delay for 1 second. Error!
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

--Quit Window
function Scenes:Quit()
QuitScene()
end

--Resume Scene
function Scenes:Resume()
ResumeScene()
end

--Pause Scene
function Scenes:Pause()
PauseScene()
end

--HowToPlayScreen
function Scenes:ShowHowToPlay()
ShowHowToPlay()
end

function Scenes:HideHowToPlay()
HideHowToPlay()
end
return Scenes