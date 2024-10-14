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

	//Base listener class
	class BaseEventListener {
	public:
		//Default virtual destructor
		virtual ~BaseEventListener() = default;

		//Event execution
		virtual void execute(std::shared_ptr<IEvent> event) = 0;
	};

	//Event listener
	template<typename T>
	class IEventListener : public BaseEventListener {
	public:
		//Default virtual destructor
		virtual ~IEventListener() = default;

		//Virtual execute event
		void execute(std::shared_ptr<IEvent> event) override {
			auto casted_event = std::dynamic_pointer_cast<T>(event);
			if(casted_event)
			executeEvent(casted_event);
		}

		//Event execution
		virtual void executeEvent(std::shared_ptr<T> event) = 0;
	};

	//Events manager
	class Manager {
	public:
		//Type def container of event listeners
		using EventListeners = std::vector<std::shared_ptr<BaseEventListener>>;

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
		void addEventListeners(std::shared_ptr<IEventListener<T>> listener) {
			event_listeners[typeid(T).name()].push_back(listener);
		}

		//Dispatch event
		template<typename T>
		void dispatchEvent(std::shared_ptr<T> event) {
			assert(event_listeners.find(typeid(T).name()) != event_listeners.end() && "Event type not found");
			auto& listeners = event_listeners.at(typeid(T).name());

			//Dispatch to listener
			for (auto& listener : listeners) {
				listener->execute(event);
			}
		}
	};
}

#endif // !M_EVENTS_HPP