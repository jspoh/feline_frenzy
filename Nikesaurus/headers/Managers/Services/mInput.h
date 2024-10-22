/*****************************************************************//**
 * \file   sysInput.h
 * \brief  input manager for engine
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef INPUT_HPP
#define INPUT_HPP

#include "mEvents.h"

namespace NIKESAURUS {
	namespace Input {

		//Key States
		enum class States {
			PRESS = NIKE_PRESS,
			RELEASE = NIKE_RELEASE,
			REPEAT = NIKE_REPEAT
		};

		//Key Event
		struct KeyEvent : public Events::IEvent {
			int code;
			States state;

			KeyEvent(int code, int keystate)
				: code{ code }, state{ static_cast<States>(keystate) } {}
		};

		//Mouse Pressed Event
		struct MouseBtnEvent : public Events::IEvent {
			int code;
			States state;

			MouseBtnEvent(int code, int btnstate)
				: code{ code }, state{ static_cast<States>(btnstate) } {}
		};

		//Mouse Moved Event
		struct MouseMovedEvent : public Events::IEvent {
			Vector2f pos;

			MouseMovedEvent(Vector2f pos)
				: pos{ pos } {}
		};

		//Mouse Scroll Event
		struct MouseScrollEvent : public Events::IEvent {
			Vector2f offset;

			MouseScrollEvent(Vector2f offset)
				: offset{ offset } {}
		};

		class NIKESAURUS_API Manager 
			:	public Events::IEventListener<KeyEvent>,
				public Events::IEventListener<MouseBtnEvent>,
				public Events::IEventListener<MouseMovedEvent>,
				public Events::IEventListener<MouseScrollEvent>
		{
		private:

			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Boolean for mouse inputs
			bool b_mouse_triggered;
			bool b_mouse_released;

			//Input mode checkss
			bool mouseTriggerCheck();
			bool mouseReleaseCheck();

			//Boolean for mouse inputs
			bool b_key_triggered;
			bool b_key_released;

			//Input mode checkss
			bool keyTriggerCheck();
			bool keyReleaseCheck();

		public:

			//Default Constructor
			Manager();

			//On Event
			void executeEvent(std::shared_ptr<KeyEvent> event) override;
			void executeEvent(std::shared_ptr<MouseBtnEvent> event) override;
			void executeEvent(std::shared_ptr<MouseMovedEvent> event) override;
			void executeEvent(std::shared_ptr<MouseScrollEvent> event) override;
		};
	}
}

#endif //!INPUT_HPP
