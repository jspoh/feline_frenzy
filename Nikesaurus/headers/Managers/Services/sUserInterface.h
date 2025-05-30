﻿/*****************************************************************//**
 * \file   sUserInterface.h
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#include "Components/cTransform.h"
#include "Components/cRender.h"
#include "Components/cAnimation.h"
#include "sInput.h"
#include "sEvents.h"
#include "sSerialization.h"
#include "Lua/sLua.h"

 //Forward declaration of friend class
namespace NIKE {
	namespace Serialization {
		class Service;
	}
}

namespace NIKE {
	namespace UI {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//UI Input States
		enum class InputStates {
			PRESSED = 0,
			TRIGGERED,
			RELEASED
		};

		inline const char* inputStateToString(InputStates state) {
			switch (state) {
			case InputStates::PRESSED:   return "Pressed";
			case InputStates::TRIGGERED: return "Triggered";
			case InputStates::RELEASED:  return "Released";
			default: return "Unknown";
			}
		}

		//UI Data Structure
		struct UIBtn {
			Entity::Type entity_id;
			bool b_hovered;
			InputStates input_state;
			std::unordered_map<std::string, Lua::Script> scripts;
			bool b_disabled;

			UIBtn() : entity_id{ 0 }, b_hovered{ false }, input_state{ InputStates::TRIGGERED }, b_disabled{ false } {};

			nlohmann::json serialize() const;

			void deserialize(nlohmann::json const& data);
		};

		//Change btn ratio event
		struct ChangeBtnTxtRatio : Events::IEvent {
			Vector2f ratio;

			ChangeBtnTxtRatio() : ratio() {}
			ChangeBtnTxtRatio(Vector2f const& ratio) : ratio{ ratio } {}
		};

		//User Interface Service
		class NIKE_API Service
			: public Events::IEventListener<Input::KeyEvent>,
			public Events::IEventListener<Input::MouseMovedEvent>,
			public Events::IEventListener<Input::MouseBtnEvent>,
			public Events::IEventListener<ChangeBtnTxtRatio> {

			//Friend class
			friend class NIKE::Serialization::Service;

		private:
			//On key btn event
			void onEvent(std::shared_ptr<Input::KeyEvent> event) override;

			//On mouse btn event
			void onEvent(std::shared_ptr<Input::MouseBtnEvent> event) override;

			//On mouse move event
			void onEvent(std::shared_ptr<Input::MouseMovedEvent> event) override;

			//On Change btn txt ratio
			void onEvent(std::shared_ptr<ChangeBtnTxtRatio> event) override;

			//Button hover check
			bool buttonHovered(Entity::Type entity) const;

			//Hover data
			struct HoverData {
				Transform::Transform btn_transform;
				Render::Text btn_text;
				Animation::Sprite btn_animate;
				bool b_hovered;
			};

			//Button hovering container
			std::unordered_map<std::string, HoverData> hover_container;

			//Unordered map of UI Entities
			std::unordered_map<std::string, UIBtn> ui_entities;

			//Data structure of state
			struct EventStates {
				bool pressed;
				bool triggered;
				bool released;
			};

			//Input events map
			std::unordered_map<int, std::pair<bool, EventStates>> input_checks;

			//Mouse Position
			Vector2f mouse_pos;

			//Button Txt Ratio
			Vector2f btn_ratio;
		public:
			//Default constructor
			Service() = default;

			//Default destructor
			~Service() = default;

			// Game overlay
			bool is_pause_initialized;
			bool is_how_to_play_overlay;
			std::unordered_map<Entity::Type, bool> inRangeEntities;

			//Create Shaped button
			Entity::Type createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Shape&& shape, Animation::Sprite&& sprite = Animation::Sprite());

			//Create texture button
			Entity::Type createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Texture&& texture, Animation::Sprite&& sprite = Animation::Sprite());

			//Destroy button
			void destroyButton(std::string const& btn_id);

			//Set button disabled
			void setButtonDisabled(std::string const& btn_id, bool disable);

			//Check button hovered
			bool isButtonHovered(std::string const& btn_id) const;

			//Check Button Clicked
			bool isButtonClicked(std::string const& btn_id, int keyorbtn_code);

			//Get all buttons
			std::unordered_map<std::string, UI::UIBtn>& getAllButtons();

			//Find a button
			UI::UIBtn* getButton(const std::string& buttonName);

			//Destroy all buttons
			void destroyAllButtons();

			//Check if entity is UI
			bool checkEntity(Entity::Type entity) const;

			//Check if UI ID has been registered
			bool checkUIEntity(std::string const& btn_id);

			// Set button script 
			// button_event is OnClick / OnHover
			void setButtonScript(std::string const& btn_id, Lua::Script const& script, std::string const& button_event);

			// Get button script
			// button_event is OnClick / OnHover
			Lua::Script getButtonScript(std::string const& btn_id, std::string const& button_event) const;

			//Set button input state
			void setButtonInputState(std::string const& btn_id, InputStates state);

			//Get button input state
			InputStates getButtonInputState(std::string const& btn_id) const;

			//UI init function
			void init();

			//UI Update function
			void update();

			// Update volume slider function
			void updateVolumeSliderPositions();
		};

		//Re-enable DLL Export warning
#pragma warning(default: 4251)
	}
}
