/*****************************************************************//**
 * \file   mEvents.h
 * \brief  Events manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef M_EVENTS_HPP
#define M_EVENTS_HPP

namespace NIKE {
	namespace Events {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Event interface
		class NIKE_API IEvent {
		private:
			bool b_event_processed;
		public:
			//Default constructor
			IEvent() : b_event_processed{ false } {}
			//Default virtual destructor
			virtual ~IEvent() = default;

			//Set event processed
			void setEventProcessed(bool processed);

			//Get event processed
			bool getEventProcessed() const;
		};

		//Base listener class
		class NIKE_API BaseEventListener {
		public:
			//Default virtual destructor
			virtual ~BaseEventListener() = default;

			//Event execution
			virtual void execute(std::shared_ptr<IEvent> event) = 0;
		};

		//Event listener
		template<typename T>
		class NIKE_API IEventListener : public BaseEventListener {
		public:
			//Default virtual destructor
			virtual ~IEventListener() = default;

			//Virtual execute event
			void execute(std::shared_ptr<IEvent> event) override {
				auto casted_event = std::dynamic_pointer_cast<T>(event);
				if (casted_event)
					onEvent(casted_event);
			}

		protected:
			//Event execution
			virtual void onEvent(std::shared_ptr<T> event) = 0;
		};

		//Events manager
		class NIKE_API Service {
		public:
			//Type def container of event listeners
			using EventListeners = std::vector<std::shared_ptr<BaseEventListener>>;

		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			//Storage of event listeners
			std::unordered_map<std::string, EventListeners> event_listeners;

		public:
			//Default constructor
			Service() = default;

			#ifdef NIKE_BUILD_DLL //Expose implementation only to NIKE Engine

			//Event Callbacks
			static void fbsize_cb(GLFWwindow* window, int width, int height);
			static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void mousebutton_cb(GLFWwindow* window, int button, int action, int mods);
			static void mousepos_cb(GLFWwindow* window, double xpos, double ypos);
			static void mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset);

			#endif //Expose implementation only to NIKE Engine

			//Add listener
			template<typename T>
			void addEventListeners(std::shared_ptr<IEventListener<T>> listener) {
				event_listeners[typeid(T).name()].push_back(listener);
			}

			//Dispatch event
			template<typename T>
			void dispatchEvent(std::shared_ptr<T> new_event) {

				//No Listener for event
				if(event_listeners.find(typeid(T).name()) == event_listeners.end()) {
					NIKEE_CORE_WARN("No one is listening for this event: " + std::string(typeid(T).name()));
					return;
				}

				auto& listeners = event_listeners.at(typeid(T).name());

				//Dispatch to listener
				for (auto& listener : listeners) {
					if (new_event->getEventProcessed())
						break;

					listener->execute(new_event);
				}
			}
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif // !M_EVENTS_HPP
