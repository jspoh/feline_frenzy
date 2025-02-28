/*****************************************************************//**
 * \file   sUserInterface.h
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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

		//UI Data Structure
		struct UIBtn {
			Entity::Type entity_id;
			bool b_hovered;
			InputStates input_state;
			Lua::Script script;

			UIBtn() : entity_id{ 0 }, b_hovered{ false }, input_state{ InputStates::TRIGGERED }, script() {};

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
			void onEvent(std::shared_ptr<Input::KeyEvent> event) override;
			void onEvent(std::shared_ptr<Input::MouseBtnEvent> event) override;
			void onEvent(std::shared_ptr<Input::MouseMovedEvent> event) override;
			void onEvent(std::shared_ptr<ChangeBtnTxtRatio> event) override;
			bool buttonHovered(Entity::Type entity) const;

			struct HoverData {
				Transform::Transform btn_transform;
				Render::Text btn_text;
				Animation::Sprite btn_animate;
				bool b_hovered;
			};

			std::unordered_map<std::string, HoverData> hover_container;
			std::unordered_map<std::string, UIBtn> ui_entities;

			struct EventStates {
				bool pressed;
				bool triggered;
				bool released;
			};

			std::unordered_map<int, std::pair<bool, EventStates>> input_checks;
			Vector2f mouse_pos;
			Vector2f btn_ratio;
		public:
			// Global UI volume variables
			float bgmVolume = 0.5f; // Global music volume (0.0 to 1.0)
			float sfxVolume = 0.5f; // Global SFX volume (0.0 to 1.0)

			Service() = default;
			~Service() = default;

			Entity::Type createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Shape&& shape, Animation::Sprite&& sprite = Animation::Sprite());
			Entity::Type createButton(std::string const& btn_id, Transform::Transform&& trans, Render::Text&& text, Render::Texture&& texture, Animation::Sprite&& sprite = Animation::Sprite());
			void destroyButton(std::string const& btn_id);
			bool isButtonHovered(std::string const& btn_id) const;
			bool isButtonClicked(std::string const& btn_id, int keyorbtn_code);
			std::unordered_map<std::string, UI::UIBtn>& getAllButtons();
			void destroyAllButtons();
			bool checkEntity(Entity::Type entity) const;
			bool checkUIEntity(std::string const& btn_id);
			void setButtonScript(std::string const& btn_id, Lua::Script const& script);
			Lua::Script getButtonScript(std::string const& btn_id) const;
			void setButtonInputState(std::string const& btn_id, InputStates state);
			InputStates getButtonInputState(std::string const& btn_id) const;
			void init();
			void update();
		};

#pragma warning(default: 4251)
	}
}
