/*****************************************************************//**
 * \file   mCoordinator.h
 * \brief  ECS Coordinator
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef M_COORDINATOR_H
#define M_COORDINATOR_H

#include "Managers/ECS/mEntity.h"
#include "Managers/ECS/mComponent.h"
#include "Managers/ECS/mSystem.h"

namespace NIKE {
	namespace Coordinator {

		class Manager {
		private:
			//ECS Managers
			std::unique_ptr<Entity::Manager> entity_manager;
			std::unique_ptr<Component::Manager> component_manager;
			std::unique_ptr<System::Manager> system_manager;
		public:

			//Default constructor
			Manager();

			/*****************************************************************//**
			* Entity Methods
			*********************************************************************/
			//Create Entity
			Entity::Type createEntity(unsigned int layer_id = 0);

			//Clone entity ( ID of clone returned )
			Entity::Type cloneEntity(Entity::Type copy);

			//Destroy Entity
			void destroyEntity(Entity::Type entity);

			//Check entity
			bool checkEntity(Entity::Type entity) const;

			//Get entity component count
			int getEntityComponentCount(Entity::Type entity) const;

			//Destroy Entity
			void destroyAllEntities();

			//Get entity count
			int getEntitiesCount();

			//Get all active entities
			std::set<Entity::Type> getAllEntities() const;

			//Set entity layer id
			void setEntityLayerID(Entity::Type entity, unsigned int layer_id);

			//Get entity layer id
			unsigned int getEntityLayerID(Entity::Type entity) const;

			/*****************************************************************//**
			* Component Methods
			*********************************************************************/
			template<typename T>
			void registerComponent() {
				component_manager->registerComponent<T>();
			}

			template<typename T>
			void removeComponent() {

				std::vector<Entity::Type> entities = component_manager->getAllComponentEntities(component_manager->getComponentType<T>());
				for (Entity::Type entity : entities) {
					//Set bit signature of component to false
					Component::Signature sign = entity_manager->getSignature(entity);
					sign.set(component_manager->getComponentType<T>(), false);
					entity_manager->setSignature(entity, sign);

					//Update entities list
					system_manager->updateEntitiesList(entity, sign, component_manager->getComponentType<T>(), false);
				}

				component_manager->removeComponent<T>();
			}

			template<typename T>
			void addEntityComponent(Entity::Type entity, T&& component) {

				//Add component
				component_manager->addEntityComponent<T>(entity, std::move(component));

				//Set bit signature of component to true
				Component::Signature sign = entity_manager->getSignature(entity);
				sign.set(component_manager->getComponentType<T>(), true);
				entity_manager->setSignature(entity, sign);

				//Update entities list
				system_manager->updateEntitiesList(entity, sign);
			}

			void addDefEntityComponent(Entity::Type entity, Component::Type type);

			template<typename T>
			void removeEntityComponent(Entity::Type entity) {
				//Remove component
				component_manager->removeEntityComponent<T>(entity);

				//Set bit signature of component to false
				Component::Signature sign = entity_manager->getSignature(entity);
				sign.set(component_manager->getComponentType<T>(), false);
				entity_manager->setSignature(entity, sign);

				//Update entities list
				system_manager->updateEntitiesList(entity, sign);
			}

			void removeEntityComponent(Entity::Type entity, Component::Type type);

			template<typename T>
			std::optional<std::reference_wrapper<T>> getEntityComponent(Entity::Type entity) {
				return component_manager->getEntityComponent<T>(entity);
			}

			std::shared_ptr<void> getEntityComponent(Entity::Type entity, Component::Type type);

			std::shared_ptr<void> getCopiedEntityComponent(Entity::Type entity, Component::Type type);

			void setEntityComponent(Entity::Type entity, Component::Type type, std::shared_ptr<void> comp);

			template<typename T>
			bool checkEntityComponent(Entity::Type entity) {
				return entity_manager->getSignature(entity).test(component_manager->getComponentType<T>());
			}

			bool checkEntityComponent(Entity::Type entity, Component::Type component_type) {
				return entity_manager->getSignature(entity).test(component_type);
			}

			template<typename T>
			Component::Type getComponentType() {
				return component_manager->getComponentType<T>();
			}

			Component::Type getComponentType(std::string const& type) {
				return component_manager->getComponentType(type);
			}

			size_t getComponentEntitiesCount(Component::Type comp_type);

			std::set<Entity::Type> getAllComponentEntities(Component::Type comp_type);

			std::unordered_map<std::string, std::shared_ptr<void>> getAllEntityComponents(Entity::Type entity) const;

			std::unordered_map<std::string, std::shared_ptr<void>> getAllCopiedEntityComponents(Entity::Type entity) const;

			std::unordered_map<std::string, Component::Type> getAllComponentTypes() const;

			size_t getComponentsCount() const;

			/*****************************************************************//**
			* System Methods
			*********************************************************************/
			template<typename T>
			std::shared_ptr<T> registerSystem(bool components_linked = true, int index = -1)
			{
				return system_manager->registerSystem<T>(components_linked, index);
			}

			template<typename T>
			void removeSystem(std::shared_ptr<T> singleton_sys = nullptr)
			{
				system_manager->removeSystem<T>();
			}

			template<typename T>
			int getSystemIndex()
			{
				return system_manager->getSystemIndex<T>();
			}

			template<typename T>
			std::string getSystemName()
			{
				return system_manager->getSystemName<T>();
			}

			template<typename T>
			void addSystemComponentType(Component::Type component)
			{
				system_manager->addComponentType<T>(component);
			}

			template<typename T>
			void setSystemState(bool state) {
				system_manager->setSystemState<T>(state);
			}

			void updateSystems();

			std::vector<std::shared_ptr<System::ISystem>>& getAllSystems();
		};
	}
}

#endif // !M_COORDINATOR_H
