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

namespace NIKESAURUS {
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
			Entity::Type createEntity();

			//Clone entity ( ID of clone returned )
			Entity::Type cloneEntity(Entity::Type copy);

			//Destroy Entity
			void destroyEntity(Entity::Type entity);

			//Get entity component count
			int getEntityComponentCount(Entity::Type entity) const;

			//Destroy Entity
			void destroyAllEntities();

			//Get entity count
			int getEntitiesCount();

			/*****************************************************************//**
			* Component Methods
			*********************************************************************/
			template<typename T>
			void registerComponent() {
				component_manager->registerComponent<T>();
			}

			template<typename T>
			void removeComponent() {

				std::vector<Entity::Type> entities = component_manager->getAllEntities<T>();
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
				system_manager->updateEntitiesList(entity, sign, component_manager->getComponentType<T>(), true);
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
				system_manager->updateEntitiesList(entity, sign, component_manager->getComponentType<T>(), false);
			}

			template<typename T>
			T& getEntityComponent(Entity::Type entity) {
				return component_manager->getEntityComponent<T>(entity);
			}

			template<typename T>
			bool checkEntityComponent(Entity::Type entity) {
				return entity_manager->getSignature(entity).test(component_manager->getComponentType<T>());
			}

			template<typename T>
			Component::Type getComponentType() {
				return component_manager->getComponentType<T>();
			}

			std::unordered_map<std::string, Component::Type> getAllComponentTypes() const;

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
			void addSystemComponentType(Component::Type component)
			{
				system_manager->addComponentType<T>(component);
			}

			void updateSystems();
		};
	}
}

#endif // !M_COORDINATOR_H
