schema = {
    label = "Start Game",
    hoverScale = 1.25,
    isEnabled = true,
    clickCount = 0
}

return function(config)
    return {
        update = function(dt)
            if config.isEnabled then
                print("Button [" .. config.label .. "] is active.")
            end
        end
    }
end
