/*****************************************************************//**
 * \file   sInput.h
 * \brief  input manager for engine
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef INPUT_HPP
#define INPUT_HPP

#include "sEvents.h"

namespace NIKE {
	namespace Input {

		//Temporary Disable DLL Export Warning
#pragma warning(disable: 4251)

//Key States
		enum class NIKE_API States {
			PRESS = NIKE_PRESS,
			RELEASE = NIKE_RELEASE,
			REPEAT = NIKE_REPEAT
		};

		//Key Event
		struct NIKE_API KeyEvent : public Events::IEvent {
			int code;
			States state;

			KeyEvent(int code, int keystate)
				: code{ code }, state{ static_cast<States>(keystate) } {}
		};

		//Mouse Event
		struct NIKE_API MouseBtnEvent : public Events::IEvent {
			int code;
			States state;

			MouseBtnEvent(int code, int btnstate)
				: code{ code }, state{ static_cast<States>(btnstate) } {}
		};

		//Mouse Moved Event
		struct NIKE_API MouseMovedEvent : public Events::IEvent {
			Vector2f window_pos;
			Vector2f world_pos;

			MouseMovedEvent(Vector2f const& window_pos, Vector2f const& world_pos)
				: window_pos{ window_pos }, world_pos{ world_pos } {}
		};

		//Mouse Scroll Event
		struct NIKE_API MouseScrollEvent : public Events::IEvent {
			Vector2f offset;

			MouseScrollEvent(Vector2f offset)
				: offset{ offset } {}
		};

		//Cursor Enter Event
		struct NIKE_API CursorEnterEvent : public Events::IEvent {
			int entered;

			CursorEnterEvent(int entered)
				: entered{ entered } {}
		};

		//Input manager for input polling
		class NIKE_API Service
			: public Events::IEventListener<KeyEvent>,
			public Events::IEventListener<MouseBtnEvent>,
			public Events::IEventListener<MouseMovedEvent>,
			public Events::IEventListener<MouseScrollEvent>,
			public Events::IEventListener<CursorEnterEvent>
		{
		private:

			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			//On input events
			void onEvent(std::shared_ptr<KeyEvent> event) override;
			void onEvent(std::shared_ptr<MouseBtnEvent> event) override;
			void onEvent(std::shared_ptr<MouseMovedEvent> event) override;
			void onEvent(std::shared_ptr<MouseScrollEvent> event) override;
			void onEvent(std::shared_ptr<CursorEnterEvent> event) override;

			//Data structure of state
			struct EventStates {
				bool polling;
				bool pressed;
				bool triggered;
				bool released;
			};

			//Set of keys
			std::set<int> curr_keys;
			std::set<int> prev_keys;

			//Mouse data structure
			struct Mouse {
				Vector2f window_pos;
				Vector2f world_pos;
				Vector2f offset;
			};

			//Mouse event
			Mouse mouse;

			struct Cursor {
				bool is_crosshair = false;
				bool cursor_entered;
			};

			Cursor cursor;

		public:

			//Default Constructor
			Service() = default;

			//Update func
			void update();

			//Key Pressed
			bool isKeyPressed(int key);

			//Key Triggered
			bool isKeyTriggered(int key);

			//Key Released
			bool isKeyReleased(int key);

			//Mouse Button Pressed
			bool isMousePressed(int key);

			//Mouse Button Triggered
			bool isMouseTriggered(int key);

			//Mouse Button Released
			bool isMouseReleased(int key);

			//Get Mouse window Position
			Vector2f getMouseWindowPos() const;

			//Get Mouse world Position
			Vector2f getMouseWorldPos() const;

			//Get Mouse Scroll Offset
			Vector2f getMouseScroll() const;

			//Get Cursor Entered
			bool getCursorEntererd() const;

			//Get Is Crosshair
			bool getCrosshair() const;

			//Set Crosshair
			void setCrosshair(bool is_crosshair);
		};

		//Re-enable DLL Export warning
#pragma warning(default: 4251)
	}
}

#endif //!INPUT_HPP
