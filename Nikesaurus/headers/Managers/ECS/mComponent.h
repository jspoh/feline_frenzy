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

namespace NIKE {
	namespace Component {

		//Component Array interface
		class IArray {
		private:
		public:
			virtual ~IArray() = default;

			//Check entity
			virtual bool checkEntity(Entity::Type entity) = 0;

			//Get entity
			virtual void* getEntityComponent(Entity::Type entity) = 0;

			//Clone entity
			virtual void cloneEntity(Entity::Type clone, Entity::Type copy) = 0;

			//Create default entity component
			virtual void createDefEntityComponent(Entity::Type entity) = 0;

			//Remove destroyed entity
			virtual void entityDestroyed(Entity::Type entity) = 0;

			// Remove component
			virtual void removeComponent(Entity::Type entity) = 0;
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
				if (component_array.find(entity) != component_array.end()) {
					throw std::runtime_error("Entity has already been added for this component.");
				}

				//Emplace component and entity
				component_array.emplace(std::piecewise_construct, std::forward_as_tuple(entity), std::forward_as_tuple(std::move(component)));
			}

			//Remove existing component
			void removeComponent(Entity::Type entity) override {
				//Check if entity is present within components to delete
				if (component_array.find(entity) == component_array.end()) {
					throw std::runtime_error("Entity not found. Unable to remove.");
				}

				//Remove entitys
				component_array.erase(entity);
			}

			//Get entity component data
			T& getComponent(Entity::Type entity) {
				//Check if entity is present within components
				if (component_array.find(entity) == component_array.end()) {
					throw std::runtime_error("Entity not found. Unable to retrieve component array.");
				}

				//Return component array
				return component_array.at(entity);
			}

			//Get all entities with component type T
			std::vector<Entity::Type> getAllEntities() {
				std::vector<Entity::Type> temp_vec;
				for (auto const& entity : component_array) {
					temp_vec.push_back(entity.first);
				}

				return temp_vec;
			}

			//Check entity
			bool checkEntity(Entity::Type entity) override {
				return component_array.find(entity) != component_array.end();
			}

			//get entity
			void* getEntityComponent(Entity::Type entity) override {
				return &component_array.at(entity);
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

			//Create default entity component
			void createDefEntityComponent(Entity::Type entity) override {
				if (component_array.find(entity) == component_array.end()) {
					component_array.emplace(std::piecewise_construct, std::forward_as_tuple(entity), std::forward_as_tuple());
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
				std::string type_name{ Utility::convertTypeString(typeid(T).name()) };

				return std::static_pointer_cast<Array<T>>(component_arrays.at(type_name));
			}

			//Component id
			Component::Type component_id;

		public:

			//Default Constructor
			Manager() : component_id{ 0 }{}

			//Register component with manager
			template<typename T>
			void registerComponent() {

				//Component type name
				std::string type_name{ Utility::convertTypeString(typeid(T).name()) };

				//Check if component has been registered before
				if (component_types.find(type_name) != component_types.end()) {
					throw std::runtime_error("Component already registered. Register failed.");
				}

				//Add component type
				component_types.emplace(std::piecewise_construct, std::forward_as_tuple(type_name), std::forward_as_tuple(component_id++));

				//Add component map
				component_arrays.emplace(std::piecewise_construct, std::forward_as_tuple(type_name), std::forward_as_tuple(std::move(std::make_shared<Array<T>>())));
			}

			template<typename T>
			void removeComponent() {
				//Component type name
				std::string type_name{ Utility::convertTypeString(typeid(T).name()) };

				//Check if component has been registered before
				if (component_types.find(type_name) == component_types.end()) {
					throw std::runtime_error("Component has not been registered before. Deregister failed.");
				}

				//Remove component type
				component_types.erase(type_name);

				//Remove component array
				component_arrays.erase(type_name);
			}

			//Add component associated with entity type
			template<typename T>
			void addEntityComponent(Entity::Type entity, T&& component) {

				//Add component
				getComponentArray<T>()->addComponent(entity, std::move(component));
			}

			//Add default entity component for entity
			void addDefEntityComponent(Entity::Type entity, Component::Type type);

			//Remove component associated with entity type
			template<typename T>
			void removeEntityComponent(Entity::Type entity) {
				//Component type name
				std::string type_name{ Utility::convertTypeString(typeid(T).name()) };

				//Remove component
				getComponentArray<T>()->removeComponent(entity);
			}

			void removeEntityComponent(Entity::Type entity, Component::Type type);

			//Retrieve component associated with entity type
			template<typename T>
			T& getEntityComponent(Entity::Type entity) {
				return getComponentArray<T>()->getComponent(entity);
			}

			//Retrieve a void* to component based on component type
			void* getEntityComponent(Entity::Type entity, Component::Type type);

			//Get Component Type
			template<typename T>
			Component::Type getComponentType() {
				//Component type name
				std::string type_name{ Utility::convertTypeString(typeid(T).name()) };

				//Check if component has been registered
				if (component_types.find(type_name) == component_types.end()) {
					throw std::runtime_error("Component not yet registered.");
				}

				//Return component type
				return component_types.at(type_name);
			}

			//Get Component Type string overload
			Component::Type getComponentType(std::string const& type);

			// Get all entities with that component
			template<typename T>
			std::vector<Entity::Type> getAllEntities() {
				return getComponentArray<T>()->getAllEntities();
			}

			//Clone entity
			void cloneEntity(Entity::Type clone, Entity::Type copy);

			//Remove entity from all components
			void entityDestroyed(Entity::Type entity);

			//Get all entity components
			std::unordered_map<std::string, void*> getAllComponents(Entity::Type entity) const;

			//Get all component types
			std::unordered_map<std::string, Component::Type> getAllComponentTypes() const;
		};
	}
}

#endif // !M_COMPONENT_HPP
