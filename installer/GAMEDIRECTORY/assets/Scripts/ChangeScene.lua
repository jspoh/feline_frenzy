------------------------------------------------------------------------------------
--file   ChangeScene.lua
--brief  Changin scene lua script
--
--author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
--date   September 2024
--All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

Scenes = {}

local ui_click_sound = "UI_Select.wav" -- Replace with your actual sound file/asset ID

-- A simple blocking wait function, note this is a busy-wait loop and will block execution.
local function wait(seconds)
    local start = os.clock()
    while os.clock() - start < seconds do
        -- TODO should change to NOT use busy-wait if possible
    end
end

--Change Scene
function Scenes:Change(args)
PlayAudio(ui_click_sound)
--wait(1)  -- Delay for 1 second. Error!
ChangeScene(args.scene)
end

--Restart Scene 
function Scenes:Restart()
PlayAudio(ui_click_sound)
RestartScene()
end

--Previous Scene 
function Scenes:Previous()
PlayAudio(ui_click_sound)
PreviousScene()
end

--Close Scene 
function Scenes:Close()
PlayAudio(ui_click_sound)
CloseScene()
end

--Quit Window
function Scenes:Quit()
PlayAudio(ui_click_sound)
QuitScene()
end

--Resume Scene
function Scenes:Resume()
PlayAudio(ui_click_sound)
ResumeScene()
end

--Pause Scene
function Scenes:Pause()
PlayAudio(ui_click_sound)
PauseScene()
end

--HowToPlayScreen
function Scenes:ShowHowToPlay()
PlayAudio(ui_click_sound)
ShowHowToPlay()
end

function Scenes:HideHowToPlay()
PlayAudio(ui_click_sound)
HideHowToPlay()
end
return Scenes