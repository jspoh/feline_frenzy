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
		mouse_pos = event->pos;
	}

	void UI::Service::onEvent(std::shared_ptr<IMGUI::ViewPortEvent> event) {
		window_pos = event->window_pos;
		window_size = event->window_size;
	}

	void UI::Service::onEvent(std::shared_ptr<ChangeBtnTxtRatio> event) {
		btn_ratio = event->ratio;
		event->setEventProcessed(true);
	}

	bool UI::Service::buttonHovered(Entity::Type entity) {
		//Get bounding box
		auto const& e_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);

		//Vertices
		std::vector<Vector2f> vert;

		//If Shape
		if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Shape>(entity)) {
			auto const& e_shape = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
			vert = NIKE_ASSETS_SERVICE->getModel(e_shape.model_id)->vertices;
			for (auto& point : vert) {
				point.x *= e_transform.scale.x * (window_size.x / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x);
				point.y *= e_transform.scale.y * (window_size.y / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
				point.x += e_transform.position.x * (window_size.x / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x);
				point.y -= e_transform.position.y * (window_size.y / NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);

				//Translate model to mouse window coords
				point.x += (window_size.x / 2.0f) + window_pos.x;
				point.y += (window_size.y / 2.0f) + window_pos.y;
			}
		}
		else {

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
		if (NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() <= 1) {
			NIKE_SCENES_SERVICE->getCurrScene()->createLayer();
		}

		//Place always place UI entity at the top layer
		ui_entities.emplace(btn_id, std::make_pair(NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() - 1), false));

		//Add components for UI
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).first, std::move(trans));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).first, std::move(text));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).first, std::move(shape));

		return ui_entities.at(btn_id).first;
	}

	Entity::Type UI::Service::createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text && text, Render::Texture && texture) {

		//Create an extra layer if there is only 1 layer
		if (NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() <= 1) {
			NIKE_SCENES_SERVICE->getCurrScene()->createLayer();
		}

		//Place always place UI entity at the top layer
		ui_entities.emplace(btn_id, std::make_pair(NIKE_ECS_MANAGER->createEntity(NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() - 1), false));

		//Add components for UI
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).first, std::move(trans));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).first, std::move(text));
		NIKE_ECS_MANAGER->addEntityComponent(ui_entities.at(btn_id).first, std::move(texture));
		
		return ui_entities.at(btn_id).first;
	}

	bool UI::Service::isButtonClicked(std::string const& btn_id, int keyorbtn_code, InputStates state) {

		if (ui_entities.at(btn_id).second) {
			input_checks[keyorbtn_code].first = true;
		}
		else {
			input_checks[keyorbtn_code].first = false;
			return false;
		}

		bool return_state = false;

		//Return state & if button hovered
		switch (state) {
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

	std::unordered_map<std::string, std::pair<Entity::Type, bool>> UI::Service::getAllButtons() const {
		return ui_entities;
	}

	void UI::Service::init() {
		window_pos = { 0.0f ,0.0f };
		window_size = { (float)NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x ,(float)NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y };
		btn_ratio = { 0.5f, 0.5f };
		std::shared_ptr<UI::Service> ui_service_wrapped(this, [](UI::Service*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<IMGUI::ViewPortEvent>(ui_service_wrapped);
	}

	void UI::Service::update() {

		//Remove inactive entities
		for (auto it = ui_entities.begin(); it != ui_entities.end();) {
			if (!NIKE_ECS_MANAGER->checkEntity(it->second.first)) {

				it = ui_entities.erase(it);
			}
			else {
				++it;
			}
		}

		//Iterate through active entities
		for (auto& entity : ui_entities) {

			//Always set UI layer entity to the last layer
			if (NIKE_ECS_MANAGER->getEntityLayerID(entity.second.first) != NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() - 1) {
				NIKE_ECS_MANAGER->setEntityLayerID(entity.second.first, NIKE_SCENES_SERVICE->getCurrScene()->getLayerCount() - 1);
			}

			//Reset all input checks to false
			for (auto& input : input_checks) {
				input.second.first = false;
			}

			//Control the text scale
			auto& e_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity.second.first);
			auto& e_text = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity.second.first);

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
			if (buttonHovered(entity.second.first)) {
				entity.second.second = true;
			}
			else {
				entity.second.second = false;
			}
		}
	}
}
