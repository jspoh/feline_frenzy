-- object
Objects = { }


function Objects:Test(args)
	local entity = args.entity
	local component = GetComponent(entity, "Combat::Health")

	-- Check if the component is valid (not nil)
	if component ~= nil then
    	-- Access the component's properties, e.g., position, scale, rotation
    	cout(component.taken_damage)
	else
	    cout("Component 'Transform' not found for entity.")
	end
end

return Objects