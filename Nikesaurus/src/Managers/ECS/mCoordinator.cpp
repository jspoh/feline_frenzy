#include "Core/stdafx.h"
#include "Managers/ECS/mCoordinator.h"

namespace NIKE {
	//Default constructor
	Coordinator::Manager::Manager()
		: entity_manager{ std::make_unique<Entity::Manager>() },
		component_manager{ std::make_unique<Component::Manager>() },
		system_manager{ std::make_unique<System::Manager>() } {}

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

	bool Coordinator::Manager::checkEntity(Entity::Type entity) const {
		return entity_manager->checkEntity(entity);
	}

	int Coordinator::Manager::getEntityComponentCount(Entity::Type entity) const{
		return entity_manager->getEntityComponentCount(entity);
	}

	void Coordinator::Manager::destroyAllEntities() {
		for (auto entity : entity_manager->getAllEntities()) {
			destroyEntity(entity);
		}
	}

	int Coordinator::Manager::getEntitiesCount() {
		return entity_manager->getEntitiesCount();
	}

	void Coordinator::Manager::updateSystems() {
		system_manager->updateSystems();
	}

	/*****************************************************************//**
	* Component Methods
	*********************************************************************/
	void Coordinator::Manager::addDefEntityComponent(Entity::Type entity, Component::Type type) {
		//Add component
		component_manager->addDefEntityComponent(entity, type);

		//Set bit signature of component to true
		Component::Signature sign = entity_manager->getSignature(entity);
		sign.set(type, true);
		entity_manager->setSignature(entity, sign);

		//Update entities list
		system_manager->updateEntitiesList(entity, sign, type, true);
	}

	std::unordered_map<std::string, Component::Type> Coordinator::Manager::getAllComponentTypes() const {
		return component_manager->getAllComponentTypes();
	}
}