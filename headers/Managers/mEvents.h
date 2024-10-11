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

namespace Events {

	//Event interface
	class IEvent {
	public:
		//Default virtual destructor
		virtual ~IEvent() = default;
	};

	//Event listener
	class IEventListener {
	public:
		//Default virtual destructor
		virtual ~IEventListener() = default;

		//Event execution
		virtual void executeEvent(std::shared_ptr<IEvent> event) = 0;
	};

	//Events manager
	class Manager {
	public:
		//Type def container of event listeners
		using EventListeners = std::vector<std::shared_ptr<IEventListener>>;

	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Storage of event listeners
		std::unordered_map<std::string, EventListeners> event_listeners;

	public:
		//Default constructor
		Manager() = default;
		
		//Add listener
		template<typename T>
		void addEventListeners(std::shared_ptr<IEventListener> listener) {
			event_listeners[typeid(T).name()].push_back(listener);
		}

		//Dispatch event
		template<typename T>
		void dispatchEvent(std::shared_ptr<T> event) {
			auto& listeners = event_listeners.at(typeid(T).name());
			for (auto& listener : listeners) {
				listener->executeEvent(event);
			}
		}
	};
}

#endif // !M_EVENTS_HPP