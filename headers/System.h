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

		//List of entities to update
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
		void setSignature(Component::Signature signature);

		//Get system signautre
		Component::Signature getSignature() const;

		//Add entity
		void addEntity(Entity::Type entity);

		//Remove Entity
		void removeEntity(Entity::Type entity);

		//Virtual Destructor
		virtual ~ISystem() = default;
	};

	//System Manager
	class Manager {
	private:

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Pointer to systems in contiguous array
		std::vector<std::shared_ptr<System::ISystem>> systems;

		//Map to systems for individual access
		std::unordered_map<std::string, std::shared_ptr<System::ISystem>> systems_map;

		//Private type casting for easy retrieval
		template<typename T>
		std::shared_ptr<T> getSystem() {
			std::string type_name{ typeid(T).name() };

			return std::static_pointer_cast<T>(systems_map.at(type_name));
		}

	public:

		//Default Constructor
		Manager() = default;

		//Index for adding system is only if
		template<typename T>
		std::shared_ptr<T> addSystem() {

			//System type name
			std::string sys_name{ typeid(T).name() };

			//Check if system has already been added
			assert(systems_map.find(sys_name) == systems_map.end() && "System already registered.");

			//System object
			std::shared_ptr<T> system{ std::make_shared<T>() };

			//Insert system at back
			systems.push_back(system);

			//Emplace shared pointer to system in map
			systems_map.emplace(std::piecewise_construct, std::forward_as_tuple(sys_name), std::forward_as_tuple(system));

			//Return system created
			return system;
		}

		//Set signature of system
		template<typename T>
		void setSignature(Component::Signature const& signature) {

			//System type name
			std::string sys_name{ typeid(T).name() };

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
			return getSystem<T>();
		}

		//Update entities list
		void updateEntitiesList(Entity::Type entity, Component::Signature e_signature);

		//Update all systems
		void updateSystems();
	};
}

#endif //!SYSTEM_HPP