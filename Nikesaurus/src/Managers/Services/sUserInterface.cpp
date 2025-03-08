/*****************************************************************//**
 * \file   sUserInterface.cpp
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sUserInterface.h"
#include "Core/Engine.h"

#include "Systems/GameLogic/sysInteraction.h"

namespace NIKE {

    nlohmann::json UI::UIBtn::serialize() const {
        return {
            {"Input_State", static_cast<int>(input_state)},
            {"Script", script.serialize()}
        };
    }

    void UI::UIBtn::deserialize(nlohmann::json const& data) {
        input_state = data.value("Input_State", UI::InputStates::TRIGGERED);
        if (data.contains("Script")) {
            script.deserialize(data["Script"]);
        }
    }

    void UI::Service::onEvent(std::shared_ptr<Input::KeyEvent> event) {
        // Check if UI is polling for this event
        if (input_checks.find(event->code) == input_checks.end()) {
            return;
        }
        // Reset all release events upon new event
        for (auto& input : input_checks) {
            input.second.second.released = false;
        }
        // Handle event states
        switch (event->state) {
        case Input::States::PRESS:
            input_checks[event->code].second.pressed = true;
            input_checks[event->code].second.triggered = true;
            input_checks[event->code].second.released = false;
            break;
        case Input::States::REPEAT:
            input_checks[event->code].second.pressed = true;
            input_checks[event->code].second.triggered = false;
            input_checks[event->code].second.released = false;
            break;
        case Input::States::RELEASE:
            input_checks[event->code].second.pressed = false;
            input_checks[event->code].second.triggered = false;
            input_checks[event->code].second.released = true;
            break;
        default:
            break;
        }
        event->setEventProcessed(true);
    }

    void UI::Service::onEvent(std::shared_ptr<Input::MouseBtnEvent> event) {
        // Check if UI is polling for this event
        if (input_checks.find(event->code) == input_checks.end()) {
            return;
        }
        // Reset all release events upon new event
        for (auto& input : input_checks) {
            input.second.second.triggered = false;
        }
        // Handle event states
        switch (event->state) {
        case Input::States::PRESS:
            input_checks[event->code].second.pressed = true;
            input_checks[event->code].second.triggered = true;
            input_checks[event->code].second.released = false;
            break;
        case Input::States::REPEAT:
            input_checks[event->code].second.pressed = true;
            input_checks[event->code].second.triggered = false;
            input_checks[event->code].second.released = false;
            break;
        case Input::States::RELEASE:
            input_checks[event->code].second.pressed = false;
            input_checks[event->code].second.triggered = false;
            input_checks[event->code].second.released = true;
            break;
        default:
            break;
        }
        event->setEventProcessed(true);
    }

    void UI::Service::onEvent(std::shared_ptr<Input::MouseMovedEvent> event) {
        // Get screen gap
        auto gaps = NIKE_WINDOWS_SERVICE->getWindow()->getViewportWindowGap();
        // Get mouse position
        mouse_pos = event->window_pos - (gaps / 2);
        // Check if mouse is over any entity currently
        std::for_each(ui_entities.begin(), ui_entities.end(),
            [&event](std::pair<std::string, UIBtn> entity) {
                if (entity.second.b_hovered) {
                    event->setEventProcessed(true);
                    return;
                }
            });
    }

    void UI::Service::onEvent(std::shared_ptr<ChangeBtnTxtRatio> event) {
        btn_ratio = event->ratio;
        event->setEventProcessed(true);
    }

    bool UI::Service::buttonHovered(Entity::Type entity) const {
        // Get bounding box from Transform component
        auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
        if (!e_transform_comp.has_value()) return false;
        auto const& e_transform = e_transform_comp.value().get();

        std::vector<Vector2f> vert;
        auto world_size = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();
        auto window_size = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();
        Vector2f scale_factor = {
            static_cast<float>(window_size.x) / static_cast<float>(world_size.x),
            static_cast<float>(window_size.y) / static_cast<float>(world_size.y)
        };

        auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
        if (e_shape_comp.has_value()) {
            auto const& e_shape = e_shape_comp.value().get();
            if (!NIKE_ASSETS_SERVICE->isAssetRegistered(e_shape.model_id)) {
                return false;
            }
            auto getVertices = [e_shape]() {
                std::vector<Assets::Vertex>& vertices =
                    NIKE_ASSETS_SERVICE->getAsset<Assets::Model>(e_shape.model_id)->vertices;
                std::vector<Vector2f> vert;
                for (const Assets::Vertex& v : vertices) {
                    vert.push_back(v.pos);
                }
                return vert;
                };
            vert = getVertices();
            for (auto& point : vert) {
                point.x *= (e_transform.scale.x * scale_factor.x);
                point.y *= (e_transform.scale.y * scale_factor.y);
                point.x += (e_transform.position.x * scale_factor.x);
                point.y -= (e_transform.position.y * scale_factor.y);
                point.x += (window_size.x / 2.0f);
                point.y += (window_size.y / 2.0f);
            }
        }
        else {
            if (!NIKE_ASSETS_SERVICE->isAssetRegistered("square-texture.model")) {
                return false;
            }
            auto getVertices = []() {
                std::vector<Assets::Vertex>& vertices =
                    NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model")->vertices;
                std::vector<Vector2f> vert;
                for (const Assets::Vertex& v : vertices) {
                    vert.push_back(v.pos);
                }
                return vert;
                };
            vert = getVertices();
            for (auto& point : vert) {
                point.x *= (e_transform.scale.x * scale_factor.x);
                point.y *= (e_transform.scale.y * scale_factor.y);
                point.x += (e_transform.position.x * scale_factor.x);
                point.y -= (e_transform.position.y * scale_factor.y);
                point.x += (window_size.x / 2.0f);
                point.y += (window_size.y / 2.0f);
            }
        }
        int intersectCount = 0;
        for (size_t i = 0; i < vert.size(); i++) {
            Vector2f v1 = vert[i];
            Vector2f v2 = vert[(i + 1) % vert.size()];
            bool isEdgeCrossing = ((v1.y > mouse_pos.y) != (v2.y > mouse_pos.y));
            if (isEdgeCrossing) {
                float intersectionX = v1.x + (mouse_pos.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
                if (mouse_pos.x < intersectionX) {
                    intersectCount++;
                }
            }
        }
        return (intersectCount % 2) == 1;
    }

    Entity::Type UI::Service::createButton(std::string const& btn_id,
        Transform::Transform&& trans, Render::Text&& text,
        Render::Shape&& shape, Animation::Sprite&& sprite) {

        if (NIKE_SCENES_SERVICE->getLayerCount() <= 1) {
            NIKE_SCENES_SERVICE->createLayer();
        }
        UIBtn btn;
        btn.entity_id = NIKE_ECS_MANAGER->createEntity();
        NIKE_METADATA_SERVICE->setEntityLayerID(btn.entity_id,
            NIKE_SCENES_SERVICE->getLayerCount() - 1);
        btn.b_hovered = false;
        ui_entities[btn_id] = btn;
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(trans));
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(text));
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(shape));
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(sprite));
        NIKE_ECS_MANAGER->addDefEntityComponent(ui_entities.at(btn_id).entity_id,
            NIKE_ECS_MANAGER->getComponentType<Animation::Base>());
        auto btn_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(ui_entities.at(btn_id).entity_id);
        auto btn_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(ui_entities.at(btn_id).entity_id);
        if (btn_transform_comp.has_value()) {
            hover_container[btn_id].btn_transform = btn_transform_comp.value().get();
            hover_container[btn_id].b_hovered = false;
            hover_container[btn_id].btn_text = btn_text_comp.value().get();
        }
        return ui_entities.at(btn_id).entity_id;
    }

    Entity::Type UI::Service::createButton(std::string const& btn_id,
        Transform::Transform&& trans, Render::Text&& text,
        Render::Texture&& texture, Animation::Sprite&& sprite) {

        if (NIKE_SCENES_SERVICE->getLayerCount() <= 1) {
            NIKE_SCENES_SERVICE->createLayer();
        }
        UIBtn btn;
        btn.entity_id = NIKE_ECS_MANAGER->createEntity();
        NIKE_METADATA_SERVICE->setEntityLayerID(btn.entity_id,
            NIKE_SCENES_SERVICE->getLayerCount() - 1);
        btn.b_hovered = false;
        ui_entities[btn_id] = btn;
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(trans));
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(text));
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(texture));
        NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(sprite));
        NIKE_ECS_MANAGER->addDefEntityComponent(ui_entities.at(btn_id).entity_id,
            NIKE_ECS_MANAGER->getComponentType<Animation::Base>());
        auto btn_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(ui_entities.at(btn_id).entity_id);
        auto btn_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(ui_entities.at(btn_id).entity_id);
        if (btn_transform_comp.has_value()) {
            hover_container[btn_id].btn_transform = btn_transform_comp.value().get();
            hover_container[btn_id].b_hovered = false;
            hover_container[btn_id].btn_text = btn_text_comp.value().get();
        }
        return ui_entities.at(btn_id).entity_id;
    }

    void UI::Service::registerUIEntity(const std::string& id, Entity::Type entity) {
        UIBtn btn;
        btn.entity_id = entity;
        btn.b_hovered = false;
        ui_entities[id] = btn;
    }

    void UI::Service::destroyButton(std::string const& btn_id) {
        auto it = ui_entities.find(btn_id);
        if (it == ui_entities.end()) {
            throw std::runtime_error("Button doesnt exist.");
        }
        NIKE_ECS_MANAGER->destroyEntity(it->second.entity_id);
        auto hover_it = hover_container.find(btn_id);
        if (hover_it != hover_container.end()) {
            hover_container.erase(hover_it);
        }
        ui_entities.erase(it);
    }

    bool UI::Service::isButtonHovered(std::string const& btn_id) const {
        auto it = ui_entities.find(btn_id);
        if (it == ui_entities.end()) {
            throw std::runtime_error("Button doesnt exist.");
        }
        return it->second.b_hovered;
    }

    // Simple hover check for slider using the entity's Transform (its center)
    bool UI::Service::sliderHovered(Entity::Type entity) const {
        auto transOpt = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
        if (!transOpt.has_value()) {
            cout << "Can't get slider transform" << endl;
            return false;
        }
        auto const& trans = transOpt.value().get();
        Vector2f halfSize = trans.scale * 0.5f;
        Vector2f mouse = NIKE_INPUT_SERVICE->getMouseWindowPos();
        if (mouse.x >= (trans.position.x - halfSize.x) &&
            mouse.x <= (trans.position.x + halfSize.x) &&
            mouse.y >= (trans.position.y - halfSize.y) &&
            mouse.y <= (trans.position.y + halfSize.y))
            return true;
        return false;
    }

    bool UI::Service::isButtonClicked(std::string const& btn_id, int keyorbtn_code) {
        if (ui_entities.at(btn_id).b_hovered) {
            input_checks[keyorbtn_code].first = true;
        }
        else {
            input_checks[keyorbtn_code].first = false;
            return false;
        }
        bool return_state = false;
        switch (ui_entities.at(btn_id).input_state) {
        case InputStates::PRESSED:
            return_state = input_checks[keyorbtn_code].second.pressed;
            input_checks[keyorbtn_code].second.pressed = false;
            return return_state;
        case InputStates::TRIGGERED:
            return_state = input_checks[keyorbtn_code].second.triggered;
            input_checks[keyorbtn_code].second.triggered = false;
            return return_state;
        case InputStates::RELEASED:
            return_state = input_checks[keyorbtn_code].second.released;
            input_checks[keyorbtn_code].second.released = false;
            return return_state;
        default:
            break;
        }
        return false;
    }

    std::unordered_map<std::string, UI::UIBtn>& UI::Service::getAllButtons() {
        return ui_entities;
    }

    void UI::Service::destroyAllButtons() {
        ui_entities.clear();
    }

    bool UI::Service::checkEntity(Entity::Type entity) const {
        for (auto const& ui_entity : ui_entities) {
            if (ui_entity.second.entity_id == entity)
                return true;
        }
        return false;
    }

    bool UI::Service::checkUIEntity(std::string const& btn_id) {
        return ui_entities.find(btn_id) != ui_entities.end();
    }

    void UI::Service::setButtonScript(std::string const& btn_id, Lua::Script const& script) {
        auto it = ui_entities.find(btn_id);
        if (it == ui_entities.end()) {
            throw std::runtime_error("Button does not exist");
        }
        it->second.script = script;
    }

    Lua::Script UI::Service::getButtonScript(std::string const& btn_id) const {
        auto it = ui_entities.find(btn_id);
        if (it == ui_entities.end()) {
            throw std::runtime_error("Button does not exist");
        }
        return it->second.script;
    }

    void UI::Service::setButtonInputState(std::string const& btn_id, InputStates state) {
        auto it = ui_entities.find(btn_id);
        if (it == ui_entities.end()) {
            throw std::runtime_error("Button does not exist");
        }
        it->second.input_state = state;
    }

    UI::InputStates UI::Service::getButtonInputState(std::string const& btn_id) const {
        auto it = ui_entities.find(btn_id);
        if (it == ui_entities.end()) {
            throw std::runtime_error("Button does not exist");
        }
        return it->second.input_state;
    }

    // Custom function to update slider functionality.
    void UI::Service::updateSliders() {
        // Process BGMSlider (the knob) and BGMSliderBar (the long bar)
        {
            // Retrieve BGMSlider and BGMSliderBar entity IDs from metadata
            auto bgmSliderOpt = NIKE_METADATA_SERVICE->getEntityByName("BGMSlider");
            auto bgmSliderBarOpt = NIKE_METADATA_SERVICE->getEntityByName("BGMSliderBar");

            if (bgmSliderOpt.has_value() && bgmSliderBarOpt.has_value())
            {
                cout << "Have value" << endl;
                Entity::Type bgmSlider = bgmSliderOpt.value();
                Entity::Type bgmSliderBar = bgmSliderBarOpt.value();

                // Retrieve Transform components for each
                auto sliderTransOpt = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bgmSlider);
                auto barTransOpt = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bgmSliderBar);

                if (sliderTransOpt.has_value() && barTransOpt.has_value())
                {
                    auto& sliderTrans = sliderTransOpt.value().get();
                    auto& barTrans = barTransOpt.value().get();

                    // Use the bar’s position.x as left bound and add scale.x as width for right bound
                    float leftBound = barTrans.position.x;
                    float rightBound = barTrans.position.x + barTrans.scale.x;

                    // IMPORTANT: use world space for comparison.
                    if (buttonHovered(bgmSlider) && NIKE_INPUT_SERVICE->isMousePressed(NIKE_MOUSE_BUTTON_LEFT))
                    {
                        // Use getMouseWorldPos() for world-space coordinates.
                        Vector2f mousePos = NIKE_INPUT_SERVICE->getMouseWorldPos();
                        sliderTrans.position.x = std::clamp(mousePos.x, leftBound, rightBound);
                        NIKEE_CORE_INFO("BGMSlider moved to x = {}", sliderTrans.position.x);
                    }

                    // Compute relative value from 0 to 1
                    float relValue = (sliderTrans.position.x - leftBound) / (rightBound - leftBound);
                    bgmVolume = relValue;

                    // Apply volume change to the Menu_BGM channel group if available
                    auto bgmGroup = NIKE_AUDIO_SERVICE->getChannelGroup("Menu_BGM");
                    if (bgmGroup) {
                        bgmGroup->setVolume(relValue);
                        NIKEE_CORE_INFO("Setting BGM volume to: {}", relValue);
                    }
                }
            }
        }

        // Process SFXSlider and SFXSliderBar similarly (if present)
        {
            auto sfxSliderOpt = NIKE_METADATA_SERVICE->getEntityByName("SFXSlider");
            auto sfxSliderBarOpt = NIKE_METADATA_SERVICE->getEntityByName("SFXSliderBar");

            if (sfxSliderOpt.has_value() && sfxSliderBarOpt.has_value())
            {
                Entity::Type sfxSlider = sfxSliderOpt.value();
                Entity::Type sfxSliderBar = sfxSliderBarOpt.value();

                auto sliderTransOpt = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(sfxSlider);
                auto barTransOpt = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(sfxSliderBar);

                if (sliderTransOpt.has_value() && barTransOpt.has_value())
                {
                    auto& sliderTrans = sliderTransOpt.value().get();
                    auto& barTrans = barTransOpt.value().get();

                    float leftBound = barTrans.position.x;
                    float rightBound = barTrans.position.x + barTrans.scale.x;

                    if (buttonHovered(sfxSlider) && NIKE_INPUT_SERVICE->isMousePressed(NIKE_MOUSE_BUTTON_LEFT))
                    {
                        Vector2f mousePos = NIKE_INPUT_SERVICE->getMouseWorldPos();
                        sliderTrans.position.x = std::clamp(mousePos.x, leftBound, rightBound);
                    }

                    float relValue = (sliderTrans.position.x - leftBound) / (rightBound - leftBound);
                    sfxVolume = relValue;

                    auto sfxGroup = NIKE_AUDIO_SERVICE->getChannelGroup("SFX");
                    if (sfxGroup)
                        sfxGroup->setVolume(relValue);
                }
            }
        }
    }

    void UI::Service::init() {
        btn_ratio = { 0.5f, 0.5f };
    }

    void UI::Service::update() {
        // Remove inactive entities
        for (auto it = ui_entities.begin(); it != ui_entities.end();) {
            if (!NIKE_ECS_MANAGER->checkEntity(it->second.entity_id)) {
                it = ui_entities.erase(it);
            }
            else {
                ++it;
            }
        }
        // Iterate through active UI entities
        for (auto& entity : ui_entities) {
            // Always ensure the UI entity is in the top layer
            if (NIKE_METADATA_SERVICE->getEntityLayerID(entity.second.entity_id) != NIKE_SCENES_SERVICE->getLayerCount() - 1) {
                NIKE_METADATA_SERVICE->setEntityLayerID(entity.second.entity_id, NIKE_SCENES_SERVICE->getLayerCount() - 1);
            }
            // Reset input checks
            for (auto& input : input_checks) {
                input.second.first = false;
            }
#ifndef NDEBUG
            if (!NIKE_LVLEDITOR_SERVICE->getGameState()) {
                return;
            }
#endif
            auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity.second.entity_id);
            if (!e_transform_comp.has_value()) continue;
            auto& e_transform = e_transform_comp.value().get();

            auto e_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity.second.entity_id);
            if (!e_text_comp.has_value()) continue;
            auto& e_text = e_text_comp.value().get();

            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity.second.entity_id);
            if (!e_animate_comp.has_value()) continue;
            auto& e_animate = e_animate_comp.value().get();

            // Clamp rotation (buttons do not rotate)
            e_transform.rotation = 0.0f;
            // Clamp rectangle size
            e_transform.scale.x = std::clamp(e_transform.scale.x, 0.0f, static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x));
            e_transform.scale.y = std::clamp(e_transform.scale.y, 0.0f, static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));

            Vector2f ratio{ e_text.size.x / e_transform.scale.x, e_text.size.y / e_transform.scale.y };
            float maxScale{ Utility::getMin(e_transform.scale.x / e_text.size.x, e_transform.scale.y / e_text.size.y) };
            float txtscale{ ratio.x - btn_ratio.x > ratio.y - btn_ratio.y
                ? std::clamp((e_transform.scale.x * btn_ratio.x) / e_text.size.x, 0.0f, maxScale)
                : std::clamp((e_transform.scale.y * btn_ratio.y) / e_text.size.y, 0.0f, maxScale) };
            e_text.scale *= txtscale;
            e_text.scale = std::clamp(e_text.scale, EPSILON, 10.0f);

            if (buttonHovered(entity.second.entity_id)) {
                if (!entity.second.b_hovered) {
                    Interaction::playOneShotSFX(entity.second.entity_id, "MenuHoverOverSFX.wav", "SFX", 1.0f, 1.0f);
                }
                entity.second.b_hovered = true;
                if (!hover_container[entity.first].b_hovered) {
                    hover_container[entity.first].btn_transform.scale = e_transform.scale;
                    hover_container[entity.first].btn_text.color = e_text.color;
                    hover_container[entity.first].btn_animate = e_animate;
                    hover_container[entity.first].b_hovered = true;
                }
                e_transform.scale = hover_container[entity.first].btn_transform.scale * 1.05f;
                e_text.color.r = hover_container[entity.first].btn_text.color.r + 0.15f;
                e_text.color.g = hover_container[entity.first].btn_text.color.g + 0.15f;
                e_text.color.b = hover_container[entity.first].btn_text.color.b + 0.15f;
                e_animate.start_index = Vector2i(1, 0);
                e_animate.end_index = Vector2i(6, 0);
                if (!entity.second.script.script_id.empty() &&
                    isButtonClicked(entity.first, NIKE_MOUSE_BUTTON_LEFT)) {
                    Interaction::playOneShotSFX(entity.second.entity_id, "UI_Select.wav", "SFX", 1.0f, 1.0f);
                    NIKE_LUA_SERVICE->executeScript(entity.second.script);
                }
            }
            else {
                entity.second.b_hovered = false;
                if (hover_container[entity.first].b_hovered) {
                    e_transform.scale = hover_container[entity.first].btn_transform.scale;
                    e_text.color = hover_container[entity.first].btn_text.color;
                    e_animate = hover_container[entity.first].btn_animate;
                    hover_container[entity.first].b_hovered = false;
                }
                input_checks[NIKE_MOUSE_BUTTON_LEFT].first = false;
            }
        }
        updateSliders();
    }
} // namespace NIKE
