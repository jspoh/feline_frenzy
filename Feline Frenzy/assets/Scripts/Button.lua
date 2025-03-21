
-- Button Object
Button = {}

-- Button Trigger Script
function Button:OnTrigger(args)

    -- Convert file path to module name
    local formatted_name = args.trigger_script:gsub("%.lua$", "")

    --Trigger script
    local status, script = pcall(require, formatted_name)
    if status and type(script) == "table" then
        script[args.trigger_func](args);
    else
        cout("Error: Failed to load script '" .. args.trigger_script .. "'.")
    end
end

-- Mouse In Button
function Button:MouseInButton(entity)

    -- Get Collider Component Of Entitys
    local component = GetComponent(entity, "Physics::Collider")

    -- Check if the component is valid
	if component ~= nil then

        -- Get local vertices
        local vert = component.Vertices;

        -- Get world mouse pos
        local mouse_pos = WindowMousePos()

        -- Local variables
        local intersectCount = 0
        local n = #vert

        -- Logic to check if mouse is within object
        for i = 1, n do
            local v1 = vert[i]
            local v2 = vert[(i % n) + 1]   -- Wrap to the start for the last edge

            -- Check if the ray intersects the edge
            local isEdgeCrossing = (v1.y > mouse_pos.y) ~= (v2.y > mouse_pos.y)
            if isEdgeCrossing then
                local intersectionX = v1.x + (mouse_pos.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y)
                if mouse_pos.x < intersectionX then
                    intersectCount = intersectCount + 1
                end
            end
        end

        -- If intersect count is odd, the point is inside
        return (intersectCount % 2) == 1

	else
	    cout("Component 'Physics::Collider ' not found for entity.")
        return false
	end
end

-- Trigger type
function Button:Trigger(type)
    if type == "Trigger" then
        return IsMouseTriggered(Key.MOUSE_LEFT)
    elseif type == "Press" then
        return IsMousePressed(Key.MOUSE_LEFT)
    elseif type == "Release" then
        return IsMouseReleased(Key.MOUSE_LEFT)
    else
        return false
    end
end

-- Button Init
function Button:Init(args)
    -- Get Collider Component Of Entitys
    local component = GetComponent(entity, "Transform::Transform")

    -- Check if the component is valid
	if component ~= nil then
        
        -- Ensure button is using screen pos
        component.use_screen_pos = true;
    end
end

-- Button Update
function Button:Update(args)
    local entity = args.entity

    -- Get Collider Component Of Entitys
    local component = GetComponent(entity, "Transform::Transform")

    -- Check if the component is valid
	if component ~= nil then
        
        -- Ensure button is using screen pos
        component.use_screen_pos = true;
    end

    --Check if mouse is within button
    if Button:MouseInButton(entity) then

    --Hover button
        
    -- Trigger button
        if(Button:Trigger(args.trigger_type)) then
            Button:OnTrigger(args)
        end
    end
end

-- Return Button object
return Button
