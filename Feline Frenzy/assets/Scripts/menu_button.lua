------------------------------------------------------------------------------------
--file   PlayAudio.lua
--brief  play audio
--
--author (100%)
--date   September 2024
--All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
-------------------------------------------------------------------------------------

Audios = {}

--Change Scene
function Audios:HoverButton(args)
	local entity = args.entity
	SetAnimationVariables(entity, 1, 0, 6, 0, 5, 0)
	PlayAudio(args.audio)

	local component = GetComponent(entity, "Transform::Transform")
	if component ~= nil then
    	-- Access the component's properties, e.g., position, scale, rotation
    	--cout(component.position.x)
    	--cout(component.position.y)
	else
	    cout("Component 'Transform' not found for entity.")
	end
end


return Audios