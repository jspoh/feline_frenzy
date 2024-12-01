/*****************************************************************//**
 * \file   sUserInterface.h
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sUserInterface.h"
#include "Core/Engine.h"

namespace NIKE {

	nlohmann::json UI::UIBtn::serialize() const {
		return	{
		{"Input_State", static_cast<int>(input_state)},
		{"Script", script.serialize()}
		};
	}

	void UI::UIBtn::deserialize(nlohmann::json const& data) {
		input_state = static_cast<UI::InputStates>(data["Input_State"].get<int>());
		script.deserialize(data["Script"]);
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

	void UI::Service::onEvent(std::shared_ptr<Input::MouseMovedEvent> event) {

		//Get mouse position
		mouse_pos = event->window_pos;

		//Check if mouse is over any entity currently
		std::for_each(ui_entities.begin(), ui_entities.end(), 
			[&event](std::pair<std::string, UIBtn> entity){
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
					point.x *= e_transform.scale.x;
					point.y *= e_transform.scale.y;
					point.x += e_transform.position.x;
					point.y -= e_transform.position.y;

					//Translate model to world coordinates
					point.x += (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / 2.0f);
					point.y += (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / 2.0f);
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
				point.x *= e_transform.scale.x;
				point.y *= e_transform.scale.y;
				point.x += e_transform.position.x;
				point.y -= e_transform.position.y;

				//Translate model to world coordinates
				point.x += (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x / 2.0f);
				point.y += (NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y / 2.0f);
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

	Entity::Type UI::Service::createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Shape&& shape) {

		//Create an extra layer if there is only 1 layer
		if (NIKE_SCENES_SERVICE->getLayerCount() <= 1) {
			NIKE_SCENES_SERVICE->createLayer();
		}

		//Place always place UI entity at the top layer
		UIBtn btn;
		btn.entity_id = NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getLayerCount() - 1);
		btn.b_hovered = false;
		ui_entities.emplace(btn_id, btn);

		//Add components for UI
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(trans));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(text));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(shape));

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

	Entity::Type UI::Service::createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Texture&& texture) {

		//Create an extra layer if there is only 1 layer
		if (NIKE_SCENES_SERVICE->getLayerCount() <= 1) {
			NIKE_SCENES_SERVICE->createLayer();
		}

		//Place always place UI entity at the top layer
		UIBtn btn;
		btn.entity_id = NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getLayerCount() - 1);
		btn.b_hovered = false;
		ui_entities.emplace(btn_id, btn);

		//Add components for UI
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(trans));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(text));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).entity_id, std::move(texture));

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

		if (ui_entities.at(btn_id).b_hovered) {
			input_checks[keyorbtn_code].first = true;
		}
		else {
			input_checks[keyorbtn_code].first = false;
			return false;
		}

		bool return_state = false;

		//Return state & if button hovered
		switch (ui_entities.at(btn_id).input_state) {
		case InputStates::PRESSED:
			return input_checks[keyorbtn_code].second.pressed;
			break;
		case InputStates::TRIGGERED:
			return_state = input_checks[keyorbtn_code].second.triggered;
			input_checks[keyorbtn_code].second.triggered = false;
			return return_state;
			break;
		case InputStates::RELEASED:
			return_state = input_checks[keyorbtn_code].second.released;
			input_checks[keyorbtn_code].second.released = false;
			return return_state;
			break;
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
		for (auto& entity : ui_entities) {

			//Always set UI layer entity to the last layer
			if (NIKE_ECS_MANAGER->getEntityLayerID(entity.second.entity_id) != NIKE_SCENES_SERVICE->getLayerCount() - 1) {
				NIKE_ECS_MANAGER->setEntityLayerID(entity.second.entity_id, NIKE_SCENES_SERVICE->getLayerCount() - 1);
			}

			//Reset all input checks to false
			for (auto& input : input_checks) {
				input.second.first = false;
			}

			//Get transform comp
			auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity.second.entity_id);
			if (!e_transform_comp.has_value()) continue;
			auto& e_transform = e_transform_comp.value().get();

			//Get text comp
			auto e_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity.second.entity_id);
			if (!e_text_comp.has_value()) continue;
			auto& e_text = e_text_comp.value().get();

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

			//Check if button is hovered
			if (buttonHovered(entity.second.entity_id)) {
				entity.second.b_hovered = true;

				//Save data before hover
				if (!hover_container[entity.first].b_hovered) {
					hover_container[entity.first].btn_transform.scale = e_transform.scale;
					hover_container[entity.first].btn_text.color = e_text.color;
					hover_container[entity.first].b_hovered = true;
				}

				//Hover
				e_transform.scale = hover_container[entity.first].btn_transform.scale * 1.05f;

				//Increase color channel
				e_text.color.r = hover_container[entity.first].btn_text.color.r + 0.15f;
				e_text.color.g = hover_container[entity.first].btn_text.color.g + 0.15f;
				e_text.color.b = hover_container[entity.first].btn_text.color.b + 0.15f;

				//Execute script for trigger
				if (!entity.second.script.script_id.empty() && isButtonClicked(entity.first, NIKE_MOUSE_BUTTON_LEFT)) {
					NIKE_LUA_SERVICE->executeScript(entity.second.script);
				}
			}
			else {
				entity.second.b_hovered = false;
				if (hover_container[entity.first].b_hovered) {
					e_transform.scale = hover_container[entity.first].btn_transform.scale;
					e_text.color = hover_container[entity.first].btn_text.color;
					hover_container[entity.first].b_hovered = false;
				}

				//Reset polling for Mouse left button
				input_checks[NIKE_MOUSE_BUTTON_LEFT].first = false;
			}
		}
	}
}
