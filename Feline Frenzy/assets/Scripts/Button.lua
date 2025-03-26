
-- Schema configs
schema = {
    hover_scale = 1.2,
    trigger_type = "trigger",
    sprite_size_x = 1,
    sprite_size_y = 1,
    sprite_start_x = 0,
    sprite_start_y = 0,
    sprite_end_x = 0,
    sprite_end_y = 0,
    animation_speed = 0.2
}

-- Button "class"
local Button = {}
Button.__index = Button

-- Button Constructor
function Button:new(configs)
    local self = setmetatable({}, Button)
    self.config = configs or {}
    return self
end

-- Button Trigger Script
function Button:OnTrigger()

    cout("Triggered")

    -- Convert file path to module name
    --local formatted_name = args.trigger_script:gsub("%.lua$", "")

    --Trigger script
    --local status, script = pcall(require, formatted_name)
   -- if status and type(script) == "table" then
        --script[args.trigger_func](args);
    --else
     --   cout("Error: Failed to load script '" .. args.trigger_script .. "'.")
   -- end
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
function Button:Trigger()
    if self.config.trigger_type == "Trigger" then
        return IsMouseTriggered(Key.MOUSE_LEFT)
    elseif self.config.trigger_type == "Press" then
        return IsMousePressed(Key.MOUSE_LEFT)
    elseif self.config.trigger_type == "Release" then
        return IsMouseReleased(Key.MOUSE_LEFT)
    else
        return IsMouseTriggered(Key.MOUSE_LEFT)
    end
end

-- Button Update
function Button:Update(entity)

    -- Get Transform Component Of Entity
    local transform = GetComponent(entity, "Transform::Transform")

    -- Check if the component is valid
	if transform ~= nil then
        -- Ensure button is using screen pos
        transform.use_screen_pos = true
    end

    -- Get Texture component of entity
    local texture = GetComponent(entity, "Render::Texture")

    -- Check if the component is valid
	if texture ~= nil then
        -- Set sprite size based on config
        texture.FrameSize.x = self.config.sprite_size_x
        texture.FrameSize.y = self.config.sprite_size_y
    end

    -- Get Texture component of entity
    local animation_sprite = GetComponent(entity, "Animation::Sprite")
    local animation_base = GetComponent(entity, "Animation::Base")

    -- Check if mouse is within button
    if self:MouseInButton(entity) then
        -- Hover Button
        if animation_sprite ~= nil and animation_base ~= nil then
            animation_sprite.StartIndex.x = self.config.sprite_start_x
            animation_sprite.StartIndex.y = self.config.sprite_start_y
            animation_sprite.EndIndex.x = self.config.sprite_end_x
            animation_sprite.EndIndex.y = self.config.sprite_end_y
            animation_base.FrameDuration = self.config.animation_speed
            animation_base.AnimationMode = 0;
        end
        
        -- Trigger button
        if(self:Trigger()) then
            self:OnTrigger()
        end
    else
        -- Reset hover animation
        if animation_sprite ~= nil and animation_base ~= nil then
            animation_sprite.CurrentIndex.x = self.config.sprite_start_x
            animation_sprite.CurrentIndex.y = self.config.sprite_start_y
            animation_base.AnimationMode = 3;
        end
    end
end

-- Object Oriented Structured, Constructs Btn & Return Lua Table
return function(configs)
    local btn = Button:new(configs)
    return {
        update = function(entity)
            btn:Update(entity)
        end
    }
end
