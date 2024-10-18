#include "../headers/Core/stdafx.h"
#include "../headers/Managers/ECS/mCoordinator.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* Entity Methods
	*********************************************************************/
	Entity::Type Coordinator::Manager::createEntity() {
		return entity_manager->createEntity();
	}

	Entity::Type Coordinator::Manager::cloneEntity(Entity::Type copy) {
		Entity::Type new_entity = entity_manager->createEntity();
		component_manager->cloneEntity(new_entity, copy);
		entity_manager->setSignature(new_entity, entity_manager->getSignature(copy));
		system_manager->cloneEntity(new_entity, copy);

		return new_entity;
	}

	void Coordinator::Manager::destroyEntity(Entity::Type entity) {

		//Destroy all data related to entity
		entity_manager->destroyEntity(entity);
		component_manager->entityDestroyed(entity);
		system_manager->entityDestroyed(entity);
	}

	void Coordinator::Manager::destroyAllEntities() {
		for (auto entity : entity_manager->getAllEntities()) {
			destroyEntity(entity);
		}
	}

	int Coordinator::Manager::getEntitiesCount() {
		return entity_manager->getEntityCount();
	}

	void Coordinator::Manager::updateSystems() {
		system_manager->updateSystems();
	}
}