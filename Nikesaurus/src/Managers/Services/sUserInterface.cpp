/*****************************************************************//**
 * \file   sUserInterface.h
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"

#include "Managers/Services/sUserInterface.h"
#include "Systems/GameLogic/sysInteraction.h"

namespace NIKE {

	nlohmann::json UI::UIBtn::serialize() const {
		nlohmann::json script_data;

		for (const auto& [event_type, script] : scripts) {
			script_data[event_type] = script.serialize(); // Serialize each script
		}

		return {
			{"Input_State", static_cast<int>(input_state)},
			{"Scripts", script_data} // Store all scripts
		};
	}

	void UI::UIBtn::deserialize(nlohmann::json const& data) {
		input_state = data.value("Input_State", UI::InputStates::TRIGGERED);

		if (data.contains("Scripts")) {
			const auto& script_data = data["Scripts"];
			for (auto it = script_data.begin(); it != script_data.end(); ++it) {
				Lua::Script script;
				script.deserialize(it.value());
				scripts[it.key()] = script; // Assign deserialized script to the correct event type
			}
		}
	}

	void UI::Service::onEvent(std::shared_ptr<Input::KeyEvent> event) {

		//Check if UI is polling for this event
		if (input_checks.find(event->code) == input_checks.end()) {
			return;
		}

		//Reset all release events upon new event
		for (auto& input : input_checks) {
			input.second.second.released = false;
		}

		//Handle event states
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

		//Check if UI is polling for this event
		if (input_checks.find(event->code) == input_checks.end()) {
			return;
		}

		//Reset all release events upon new event
		for (auto& input : input_checks) {
			input.second.second.triggered = false;
		}

		//Handle event states
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

		//Get screen gap
		auto gaps = NIKE_WINDOWS_SERVICE->getWindow()->getViewportWindowGap();

		//Get mouse position
		mouse_pos = event->window_pos - (gaps / 2);

		//Check if mouse is over any entity currently
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
		//Get bounding box
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) return false;
		auto const& e_transform = e_transform_comp.value().get();

		//Vertices
		std::vector<Vector2f> vert;

		//Get World size
		auto world_size = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();

		//Get window size
		auto window_size = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();

		//Calculate scale factor
		Vector2f scale_factor = { static_cast<float>(window_size.x) / static_cast<float>(world_size.x), static_cast<float>(window_size.y) / static_cast<float>(world_size.y) };

		//If Shape
		auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
		if (e_shape_comp.has_value()) {
			auto const& e_shape = e_shape_comp.value().get();

			//Check if model is registered
			if (!NIKE_ASSETS_SERVICE->isAssetRegistered(e_shape.model_id)) {
				return false;
			}

			auto getVertices = [e_shape]() {
				std::vector<Assets::Vertex>& vertices = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>(e_shape.model_id)->vertices;

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

				//Translate model to world coordinates
				point.x += (window_size.x / 2.0f);
				point.y += (window_size.y / 2.0f);
			}
		}
		else {
			if (!NIKE_ASSETS_SERVICE->isAssetRegistered("square-texture.model")) {
				return false;
			}

			auto getVertices = []() {
				std::vector<Assets::Vertex>& vertices = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model")->vertices;

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

				//Translate model to world coordinates
				point.x += (window_size.x / 2.0f);
				point.y += (window_size.y / 2.0f);
			}
		}

		//Calculate vertices intersection with mouse
		int intersectCount = 0;

		for (size_t i = 0; i < vert.size(); i++) {
			Vector2f v1 = vert[i];
			Vector2f v2 = vert[(i + 1) % vert.size()];  // Wrap to the start for the last edge

			// Check if the ray intersects the edge
			bool isEdgeCrossing = ((v1.y > mouse_pos.y) != (v2.y > mouse_pos.y));
			if (isEdgeCrossing) {
				float intersectionX = v1.x + (mouse_pos.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
				if (mouse_pos.x < intersectionX) {
					intersectCount++;
				}
			}
		}

		// If intersect count is odd, the point is inside
		return (intersectCount % 2) == 1;
	}

	Entity::Type UI::Service::createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Shape&& shape, Animation::Sprite&& sprite) {

		//Create an extra layer if there is only 1 layer
		if (NIKE_SCENES_SERVICE->getLayerCount() <= 1) {
			NIKE_SCENES_SERVICE->createLayer();
		}

		//Place always place UI entity at the top layer
		UIBtn btn;
		btn.entity_id = NIKE_ECS_MANAGER->createEntity();
		NIKE_METADATA_SERVICE->setEntityLayerID(btn.entity_id, NIKE_SCENES_SERVICE->getLayerCount() - 1);
		btn.b_hovered = false;
		ui_entities[btn_id] = btn;

		//Add components for UI
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(trans));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(text));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(shape));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(sprite));
		NIKE_ECS_MANAGER->addDefEntityComponent(ui_entities.at(btn_id).entity_id, NIKE_ECS_MANAGER->getComponentType<Animation::Base>());

		auto btn_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(ui_entities.at(btn_id).entity_id);
		auto btn_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(ui_entities.at(btn_id).entity_id);

		//Add transform into hover container
		if (btn_transform_comp.has_value()) {
			hover_container[btn_id].btn_transform = btn_transform_comp.value().get();
			hover_container[btn_id].b_hovered = false;
			hover_container[btn_id].btn_text = btn_text_comp.value().get();
		}

		return ui_entities.at(btn_id).entity_id;
	}

	Entity::Type UI::Service::createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Texture&& texture, Animation::Sprite&& sprite) {

		//Create an extra layer if there is only 1 layer
		if (NIKE_SCENES_SERVICE->getLayerCount() <= 1) {
			NIKE_SCENES_SERVICE->createLayer();
		}

		//Place always place UI entity at the top layer
		UIBtn btn;
		btn.entity_id = NIKE_ECS_MANAGER->createEntity();
		NIKE_METADATA_SERVICE->setEntityLayerID(btn.entity_id, NIKE_SCENES_SERVICE->getLayerCount() - 1);
		btn.b_hovered = false;
		ui_entities[btn_id] = btn;

		//Add components for UI
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(trans));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(text));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(texture));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(sprite));
		NIKE_ECS_MANAGER->addDefEntityComponent(ui_entities.at(btn_id).entity_id, NIKE_ECS_MANAGER->getComponentType<Animation::Base>());

		auto btn_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(ui_entities.at(btn_id).entity_id);
		auto btn_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(ui_entities.at(btn_id).entity_id);

		//Add transform into hover container
		if (btn_transform_comp.has_value()) {
			hover_container[btn_id].btn_transform = btn_transform_comp.value().get();
			hover_container[btn_id].b_hovered = false;
			hover_container[btn_id].btn_text = btn_text_comp.value().get();
		}

		return ui_entities.at(btn_id).entity_id;
	}

	void UI::Service::destroyButton(std::string const& btn_id) {
		//Check if button exists
		auto it = ui_entities.find(btn_id);
		if (it == ui_entities.end()) {
			throw std::runtime_error("Button doesnt exist.");
		}

		//Destroy entity
		NIKE_ECS_MANAGER->destroyEntity(it->second.entity_id);

		//Check if hovering button exists
		auto hover_it = hover_container.find(btn_id);
		if (hover_it != hover_container.end()) {
			hover_it = hover_container.erase(hover_it);
		}

		//Erase button
		it = ui_entities.erase(it);
	}

	bool UI::Service::isButtonHovered(std::string const& btn_id) const {

		//Check if button exists
		auto it = ui_entities.find(btn_id);
		if (it == ui_entities.end()) {
			throw std::runtime_error("Button doesnt exist.");
		}

		//Return hover state
		return it->second.b_hovered;
	}

	bool UI::Service::isButtonClicked(std::string const& btn_id, int keyorbtn_code) {

		// Get the entity once instead of multiple calls
		auto it = ui_entities.find(btn_id);

		if (it == ui_entities.end()) {
			return false; // Button not found
		}

		auto& entity = it->second;
		// If not hovered, return false and update input check
		if (!entity.b_hovered) {
			input_checks[keyorbtn_code].first = false;
			return false;
		}

		input_checks[keyorbtn_code].first = true;

		bool return_state = false;

		//Return state & if button hovered
		switch (entity.input_state) {
		case InputStates::PRESSED:
			return_state = input_checks[keyorbtn_code].second.pressed;
			break;
		case InputStates::TRIGGERED:
			return_state = input_checks[keyorbtn_code].second.triggered;

			break;
		case InputStates::RELEASED:
			return_state = input_checks[keyorbtn_code].second.released;
			break;
		default:
			break;
		}

		// Reset input
		input_checks[keyorbtn_code].second.pressed = false;
		input_checks[keyorbtn_code].second.triggered = false;
		input_checks[keyorbtn_code].second.released = false;

		return return_state;
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

	void UI::Service::setButtonScript(std::string const& btn_id, Lua::Script const& script, std::string const& button_event) {
		auto it = ui_entities.find(btn_id);
		if (it == ui_entities.end()) {
			throw std::runtime_error("Button does not exist");
		}

		it->second.scripts[button_event] = script;
	}

	Lua::Script UI::Service::getButtonScript(std::string const& btn_id, std::string const& button_event) const {
		auto it = ui_entities.find(btn_id);
		if (it == ui_entities.end()) {
			throw std::runtime_error("Button does not exist");
		}

		auto script_it = it->second.scripts.find(button_event);
		if (script_it == it->second.scripts.end()) {
			throw std::runtime_error("Script for '" + button_event + "' does not exist");
		}

		return script_it->second;
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

	void UI::Service::init() {
		btn_ratio = { 0.5f, 0.5f };
	}

	void UI::Service::update() {
		//Remove inactive entities
		for (auto it = ui_entities.begin(); it != ui_entities.end();) {
			if (!NIKE_ECS_MANAGER->checkEntity(it->second.entity_id)) {

				it = ui_entities.erase(it);
			}
			else {
				++it;
			}
		}

		//Iterate through active entities
		for (auto& [entity_name, entity] : ui_entities) {

			////Always set UI layer entity to the last layer
			//if (NIKE_METADATA_SERVICE->getEntityLayerID(entity.second.entity_id) != NIKE_SCENES_SERVICE->getLayerCount() - 1) {
			//	NIKE_METADATA_SERVICE->setEntityLayerID(entity.second.entity_id, NIKE_SCENES_SERVICE->getLayerCount() - 1);
			//}

#ifndef NDEBUG
			if (!NIKE_LVLEDITOR_SERVICE->getGameState()) {
				return;
			}
#endif
			// Reset all input checks once per frame
			for (auto& input : input_checks) {
				input.second.first = false;
			}

			//Get transform comp
			auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity.entity_id);
			if (!e_transform_comp.has_value()) continue;
			auto& e_transform = e_transform_comp.value().get();

			//Get text comp
			auto e_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity.entity_id);
			if (!e_text_comp.has_value()) continue;
			auto& e_text = e_text_comp.value().get();

			//Get animation comp
			auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity.entity_id);
			if (!e_animate_comp.has_value()) continue;
			auto& e_animate = e_animate_comp.value().get();



			//Clamp rotation ( Disable rotating buttons for now )
			e_transform.rotation = 0.0f;

			//Clamp Rectangle Size
			e_transform.scale.x = std::clamp(e_transform.scale.x, 0.0f, static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x));
			e_transform.scale.y = std::clamp(e_transform.scale.y, 0.0f, static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y));

			//Text size to transform ratio
			Vector2f ratio{ e_text.size.x / e_transform.scale.x, e_text.size.y / e_transform.scale.y };
			float maxScale{ Utility::getMin(e_transform.scale.x / e_text.size.x, e_transform.scale.y / e_text.size.y) };
			float txtscale{ ratio.x - btn_ratio.x > ratio.y - btn_ratio.y
				? std::clamp((e_transform.scale.x * btn_ratio.x) / e_text.size.x, 0.0f, maxScale)
				: std::clamp((e_transform.scale.y * btn_ratio.y) / e_text.size.y, 0.0f, maxScale) };

			e_text.scale *= txtscale;

			//Clamp scale
			e_text.scale = std::clamp(e_text.scale, EPSILON, 10.0f);

			// Lambda to execute scripts if they exist
			auto executeScriptIfExists = [&](auto& ent, const std::string& scriptName) {
				auto it = ent.scripts.find(scriptName);
				if (it != ent.scripts.end() && !it->second.script_id.empty()) {
					it->second.named_args["entity"] = ent.entity_id; // Set entity default
					NIKE_LUA_SERVICE->executeScript(it->second);
				}
				};

			// Hovering logic
			if (buttonHovered(entity.entity_id)) {

				entity.b_hovered = true;

				if (!hover_container[entity_name].b_hovered) {
					// Store original properties BEFORE modifying them
					hover_container[entity_name].btn_transform.scale = e_transform.scale;
					hover_container[entity_name].btn_text.color = e_text.color;
					hover_container[entity_name].btn_animate = e_animate;
					hover_container[entity_name].b_hovered = true;

					executeScriptIfExists(entity, "OnHover");
				}

				// Apply hover effects
				e_transform.scale = hover_container[entity_name].btn_transform.scale * 1.05f;

				e_text.color.r = std::clamp(hover_container[entity_name].btn_text.color.r + 0.15f, 0.0f, 1.0f);
				e_text.color.g = std::clamp(hover_container[entity_name].btn_text.color.g + 0.15f, 0.0f, 1.0f);
				e_text.color.b = std::clamp(hover_container[entity_name].btn_text.color.b + 0.15f, 0.0f, 1.0f);


				// Execute click script
				if (isButtonClicked(entity_name, NIKE_MOUSE_BUTTON_LEFT)) {
					input_checks[NIKE_MOUSE_BUTTON_LEFT].first = false;
					executeScriptIfExists(entity, "OnClick");
				}
			}
			else {
				entity.b_hovered = false;
				if (hover_container[entity_name].b_hovered) {
					e_transform.scale = hover_container[entity_name].btn_transform.scale;
					e_text.color = hover_container[entity_name].btn_text.color;
					e_animate = hover_container[entity_name].btn_animate;
					hover_container[entity_name].b_hovered = false;
				}

				//Reset polling for Mouse left button
				input_checks[NIKE_MOUSE_BUTTON_LEFT].first = false;
			}

		}
	}
}
