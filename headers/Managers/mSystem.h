/*****************************************************************//**
 * \file   mSystem.h
 * \brief  System manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef M_SYSTEM_HPP
#define M_SYSTEM_HPP

namespace System {

	//System interface
	class ISystem {
	protected:
		//System signature
		Component::Signature system_signature;

		//Component Types
		std::set<Component::Type> component_types;

		//List of entities to update
		std::set<Entity::Type> entities;

		//Linked components will use signature for filtering entities
		//Unlinked components will check if any of the system components
		//are present within entity. If present, entity is added to the entities list
		bool b_components_linked;

		//System active
		bool b_system_active;
	public:

		//Constructor
		ISystem() : system_signature{}, b_components_linked{ true }, b_system_active { true } {}

		//Virtual Init
		virtual void init() {}

		//Pure virtual function to be implemented in inherited class
		virtual void update() = 0;

		//Set components linked mode
		void setComponentsLinked(bool state);

		//Get components linked mode
		bool getComponentsLinked() const;

		//Set System active
		void setActiveState(bool state);

		//Get System active
		bool getActiveState() const;

		//Add component type
		void addComponentType(Component::Type component);

		//Remove component type
		void removeComponentType(Component::Type component);

		//Check if component is present
		bool checkComponentType(Component::Type component) const;

		//Get system signautre
		Component::Signature getSignature() const;

		//Add entity
		void addEntity(Entity::Type entity);

		//Remove Entity
		void removeEntity(Entity::Type entity);

		//Check if entity is present
		bool checkEntity(Entity::Type entity) const;

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
		std::shared_ptr<T> registerSystem(std::shared_ptr<T> singleton_sys = nullptr) {

			//System type name
			std::string sys_name{ typeid(T).name() };

			//Check if system has already been added
			assert(systems_map.find(sys_name) == systems_map.end() && "System already registered.");

			//System object
			std::shared_ptr<T> system;
			
			if (singleton_sys) {
				system = singleton_sys;
			}
			else {
				system = std::make_shared<T>();
			}

			//Init system
			system->init();

			//Insert system at back
			systems.push_back(system);

			//Emplace shared pointer to system in map
			systems_map.emplace(std::piecewise_construct, std::forward_as_tuple(sys_name), std::forward_as_tuple(system));

			//Return system created
			return system;
		}

		//Set system state
		template<typename T>
		void setSystemState(bool state) {
			//System type name
			std::string sys_name{ typeid(T).name() };

			//Check if system is present
			assert(systems_map.find(sys_name) != systems_map.end() && "System not registered. Setting of state failed");

			//Remove system
			systems_map.at(sys_name)->setActiveState(state);
		}

		//Set signature of system
		template<typename T>
		void addComponentType(Component::Type component) {

			//System type name
			std::string sys_name{ typeid(T).name() };

			//Set signature of system
			systems_map.at(sys_name)->addComponentType(component);
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

		//Update entities list based on signature
		void updateEntitiesList(Entity::Type entity, Component::Signature e_signature, Component::Type component, bool b_component_added);

		//Remove entity from all systems
		void entityDestroyed(Entity::Type entity);

		//Update all systems
		void updateSystems();
	};
}

#endif //!M_SYSTEM_HPP