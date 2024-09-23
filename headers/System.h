/*****************************************************************//**
 * \file   System.h
 * \brief  System manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

namespace System {

	//System interface
	class ISystem {
	private:
		//System signature
		Component::Signature c_id;

		//System entities
		std::set<Entity::Type> entities;

		//System active
		bool b_system_active;
	public:

		//Constructor
		ISystem() : c_id{}, b_system_active { true } {}

		//Virtual Init
		virtual void init() {}

		//Pure virtual function to be implemented in inherited class
		virtual void update() = 0;

		//Set System active
		void setActiveState(bool state);

		//Get System active
		bool getActiveState() const;

		//Set system signature
		void setSignaure(Component::Signature signature);

		//Get system signautre
		Component::Signature getSignature() const;

		//Virtual Destructor
		virtual ~ISystem() = default;
	};

	//System Manager
	class Manager {
	private:
		//Private Default Constructor
		Manager() = default;

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Pointer to engine systems in contiguous array
		std::vector<std::shared_ptr<System::ISystem>> systems;

		//Map to systems for individual access
		std::unordered_map<std::string, std::shared_ptr<System::ISystem>> systems_map;

	public:

		//Index for adding system is only if
		template<typename T>
		std::shared_ptr<T> addSystem(size_t index = std::string::npos) {

			//System type name
			std::string sys_name{ typeid(T).name };

			//System object
			std::shared_ptr<T> system{ std::make_shared<T>() };

			//Check if system has already been added
			if (systems_map.find(sys_name) != systems_map.end()) {
				return systems_map.at(sys_name);
			}

			//Check for index
			if (index == std::string::npos && index >= systems.size()) {
				//Insert system at back
				systems.push_back(system);
			}
			else {
				//Insert system
				auto it = systems.begin();
				systems.insert(it + index, system);
			}

			//Emplace shared pointer to system in map
			systems_map.emplace(std::piecewise_construct, std::forward_as_tuple(sys_name), std::forward_as_tuple(std::move(system)));
		}

		//Set signature of system
		template<typename T>
		void setSignature(Component::Signature const& signature) {

			//System type name
			std::string sys_name{ typeid(T).name };

			//Set signature of system
			systems_map.at(sys_name)->setSignaure(signature);
		}

		/**
		 * Access system already within core engine
		 *
		 * \param sys_identifier	system identifier ( eg. PhysicsSystem = "PhysicsSystem" )
		 *
		 */
		template<typename T>
		std::shared_ptr<T> accessSystem() {
			//System type name
			std::string sys_name{ typeid(T).name };

			return systems_map.at(sys_name);
		}

		//Update all systems
		void updateSystems();
	};
}

#endif //!SYSTEM_HPP