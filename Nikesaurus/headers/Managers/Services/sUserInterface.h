/*****************************************************************//**
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
#include "sInput.h"
#include "sEvents.h"
#include "sSerialization.h"

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

		//User Interface Service
		class NIKE_API Service 
			:	public Events::IEventListener<Input::KeyEvent>,
				public Events::IEventListener<Input::MouseMovedEvent>,
				public Events::IEventListener<Input::MouseBtnEvent> {

		//Friend class
		friend class NIKE::Serialization::Service;

		private:

			//Unordered map of UI Entities
			std::unordered_map<std::string, std::pair<Entity::Type, bool>> ui_entities;

			//On key btn event
			void onEvent(std::shared_ptr<Input::KeyEvent> event) override;

			//On mouse btn event
			void onEvent(std::shared_ptr<Input::MouseBtnEvent> event) override;

			//On mouse move event
			void onEvent(std::shared_ptr<Input::MouseMovedEvent> event) override;

			//Button hover check
			bool buttonHovered(Entity::Type entity);

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
		public:
			//Default constructor
			Service() = default;

			//Default destructor
			~Service() = default;

			//Create Shaped button
			Entity::Type createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Shape&& shape);

			//Create texture button
			Entity::Type createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Texture&& texture);

			//Check Button Clicked
			bool isButtonClicked(std::string const& btn_id, int keyorbtn_code, InputStates state);

			//Check if entity is a button
			std::unordered_map<std::string, std::pair<Entity::Type, bool>> getAllButtons() const;

			//UI Update function
			void update();
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}
