/*****************************************************************//**
 * \file   Component.h
 * \brief  Component manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

namespace Component {

	//Component Array interface
	class IArray {
	private:
	public:
		virtual ~IArray() = default;
	};

	//Component Array ( Entity Map of unique components of type T )
	template<typename T>
	class Array : public IArray {
	private:

		//Unique component of same type identified by Entity type
		std::unordered_map<Entity::Type, T> component_array;

	public:

		//Default constructor
		Array() = default;

		//Add new component
		void addComponent(Entity::Type entity, T&& component);

		//Remove existing component
		void removeComponent(Entity::Type entity);

		//Get entity component data
		T& getComponent(Entity::Type entity);
	};

	//Manager of the component Array
	class Manager {
	private:

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Map to component type ( used for signature setting )
		std::unordered_map<std::string, Component::Type> component_types;

		//Map to array of component type
		std::unordered_map<std::string, std::shared_ptr<IArray>> component_arrays;

		//Private type casting for easy retrieval
		template<typename T>
		std::shared_ptr<Array<T>> getComponentArray() {
			std::string type_name{ typeid(T).name() };

			return std::static_pointer_cast<Array<T>>(component_arrays.at(type_name));
		}

	public:

		//Default Constructor
		Manager() = default;

		//Register component with manager
		template<typename T>
		void registerComponent() {

			//Static component count variable
			static Component::Type component_count{ 0 };

			//Component type name
			std::string type_name{ typeid(T).name() };

			//Check if component has been registered before
			assert(component_types.find(type_name) == component_types.end() && "Component already registered.");

			//Add component type
			component_types.emplace(std::piecewise_construct, std::forward_as_tuple(type_name), std::forward_as_tuple(component_count));

			//Add component map
			component_arrays.emplace(std::piecewise_construct, std::forward_as_tuple(type_name), std::forward_as_tuple(std::move(std::make_shared<Array<T>>())));

			//Increment component count variable
			component_count++;
		}

		//Add component associated with entity type
		template<typename T>
		void addEntityComponent(Entity::Type entity, T&& component) {
			//Component type name
			std::string type_name{ typeid(T).name() };

			//Add component
			getComponentArray<T>()->addComponent(entity, std::move(component));
		}

		//Remove component associated with entity type
		template<typename T>
		void removeEntityComponent(Entity::Type entity) {
			//Component type name
			std::string type_name{ typeid(T).name() };

			//Remove component
			getComponentArray<T>()->removeComponent(entity);
		}

		//Retrieve component associated with entity type
		template<typename T>
		T& getEntityComponent(Entity::Type entity) {
			return getComponentArray<T>()->getComponent(entity);
		}
		
		//Get Component Type
		template<typename T>
		Component::Type getComponentType() {
			//Component type name
			std::string type_name{ typeid(T).name() };

			//Check if component has been registered
			assert(component_types.find(type_name) != component_types.end() && "Component not yet registered.");

			return component_types.at(type_name);
		}
	};
}

#endif // !COMPONENT_HPP
