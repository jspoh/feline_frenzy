/*****************************************************************//**
 * \file   mComponent.h
 * \brief  Component manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef M_COMPONENT_HPP
#define M_COMPONENT_HPP

namespace NIKESAURUS {
	namespace Component {

		//Component Signature Type
		using Type = uint8_t;

		//Max components to be stored in a signature
		const Type MAX = 64;

		//Nested components signature
		using Signature = std::bitset<MAX>;

		//Component Array interface
		class IArray {
		private:
		public:
			virtual ~IArray() = default;

			//Clone entity
			virtual void cloneEntity(Entity::Type clone, Entity::Type copy) = 0;

			//Remove destroyed entity
			virtual void entityDestroyed(Entity::Type entity) = 0;
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
			void addComponent(Entity::Type entity, T&& component) {
				//Check if entity has already been added
				assert(component_array.find(entity) == component_array.end() && "Entity has already been added for this component.");

				//Emplace component and entity
				component_array.emplace(std::piecewise_construct, std::forward_as_tuple(entity), std::forward_as_tuple(std::move(component)));
			}

			//Remove existing component
			void removeComponent(Entity::Type entity) {
				//Check if entity is present within components to delete
				assert(component_array.find(entity) != component_array.end() && "Entity not found. Unable to remove.");

				//Remove entitys
				component_array.erase(entity);
			}

			//Get entity component data
			T& getComponent(Entity::Type entity) {
				//Check if entity is present within components
				assert(component_array.find(entity) != component_array.end() && "Entity not found. Unable to retrieve component array.");

				//Return component array
				return component_array.at(entity);
			}

			//Check if entity component is present
			bool checkComponent(Entity::Type entity) const {
				return component_array.find(entity) != component_array.end();
			}

			//Get all entities with component type T
			std::vector<Entity::Type> getAllEntities() {
				std::vector<Entity::Type> temp_vec;
				for (auto const& entity : component_array) {
					temp_vec.push_back(entity.first);
				}

				return temp_vec;
			}

			//Clone entity
			void cloneEntity(Entity::Type clone, Entity::Type copy) override {
				//Clone entity
				if (component_array.find(copy) != component_array.end()) {
					T temp_copy = component_array.at(copy);

					//Emplace component and entity
					component_array.emplace(std::piecewise_construct, std::forward_as_tuple(clone), std::forward_as_tuple(std::move(temp_copy)));
				}
			}

			//Remove destroyed entity
			void entityDestroyed(Entity::Type entity) override {
				if (component_array.find(entity) != component_array.end())
					removeComponent(entity);
			}
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

			//Component id
			Component::Type component_id{ 0 };

		public:

			//Default Constructor
			Manager() = default;

			//Register component with manager
			template<typename T>
			void registerComponent() {

				//Component type name
				std::string type_name{ typeid(T).name() };

				//Check if component has been registered before
				assert(component_types.find(type_name) == component_types.end() && "Component already registered. Register failed.");

				//Add component type
				component_types.emplace(std::piecewise_construct, std::forward_as_tuple(type_name), std::forward_as_tuple(component_id++));

				//Add component map
				component_arrays.emplace(std::piecewise_construct, std::forward_as_tuple(type_name), std::forward_as_tuple(std::make_shared<Array<T>>()));
			}

			template<typename T>
			void removeComponent() {
				//Component type name
				std::string type_name{ typeid(T).name() };

				//Check if component has been registered before
				assert(component_types.find(type_name) != component_types.end() && "Component has not been registered before. Deregister failed.");

				//Remove component type
				component_types.erase(type_name);

				//Remove component array
				component_arrays.erase(type_name);
			}

			//Add component associated with entity type
			template<typename T>
			void addEntityComponentObj(Entity::Type entity, T&& component) {
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

			//Check if entity component is present
			template<typename T>
			bool checkEntityComponent(Entity::Type entity) {
				return getComponentArray<T>()->checkComponent(entity);
			}

			//Get Component Type
			template<typename T>
			Component::Type getComponentType() {
				//Component type name
				std::string type_name{ typeid(T).name() };

				//Check if component has been registered
				assert(component_types.find(type_name) != component_types.end() && "Component not yet registered.");

				//Return component type
				return component_types.at(type_name);
			}

			//Get all entities
			template<typename T>
			std::vector<Entity::Type> getAllEntities() {
				return getComponentArray<T>()->getAllEntities();
			}

			//Clone entity
			void cloneEntity(Entity::Type clone, Entity::Type copy);

			//Remove entity from all components
			void entityDestroyed(Entity::Type entity);
		};
	}
}

#endif // !M_COMPONENT_HPP
