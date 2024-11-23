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
	Entity::Type Coordinator::Manager::createEntity(unsigned int layer_id) {
		return entity_manager->createEntity(layer_id);
	}

	Entity::Type Coordinator::Manager::cloneEntity(Entity::Type copy) {
		Entity::Type new_entity = entity_manager->createEntity(entity_manager->getLayerID(copy));
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

	std::set<Entity::Type> Coordinator::Manager::getAllEntities() const {
		return entity_manager->getAllEntities();
	}

	void Coordinator::Manager::setEntityLayerID(Entity::Type entity, unsigned int layer_id) {
		entity_manager->setLayerID(entity, layer_id);
	}

	unsigned int Coordinator::Manager::getEntityLayerID(Entity::Type entity) const {
		return entity_manager->getLayerID(entity);
	}

	void Coordinator::Manager::updateSystems() {
		system_manager->updateSystems();
	}

	std::vector<std::shared_ptr<System::ISystem>>& Coordinator::Manager::getAllSystems() {
		return system_manager->getAllSystems();
	}

	void Coordinator::Manager::markEntityForDeletion(Entity::Type entity) {
		if (std::find(entities_to_destroy.begin(), entities_to_destroy.end(), entity) == entities_to_destroy.end()) {
			entities_to_destroy.push_back(entity);
		}
		else {
			NIKEE_CORE_INFO("Entity already marked for deletion");
		}
	}

	void Coordinator::Manager::destroyMarkedEntities() {
		//NIKEE_CORE_INFO("IN DESTROY MARKED ENTITIES");
		for (auto entity : entities_to_destroy) {
			if (checkEntity(entity)) {
				//NIKEE_CORE_INFO("DESTROYING AN ENTITY");
				destroyEntity(entity);
				//NIKEE_CORE_INFO("AFTER DESTROYING AN ENTITY");
			}
			else {
				NIKEE_CORE_INFO("Entity {} failed the checkEntity() check.", entity);
				continue;
			}
		}
		entities_to_destroy.clear();
	}

	std::vector<Entity::Type> Coordinator::Manager::getEntitiesToDestroy() const{
		return entities_to_destroy;
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
		system_manager->updateEntitiesList(entity, sign);
	}

	void Coordinator::Manager::removeEntityComponent(Entity::Type entity, Component::Type type) {
		// Remove component based on Component::Type directly
		component_manager->removeEntityComponent(entity, type);

		// Set bit signature of component to false
		Component::Signature sign = entity_manager->getSignature(entity);
		sign.set(type, false);
		entity_manager->setSignature(entity, sign);

		// Update entities list
		system_manager->updateEntitiesList(entity, sign);
	}

	std::shared_ptr<void> Coordinator::Manager::getEntityComponent(Entity::Type entity, Component::Type type) {
		return component_manager->getEntityComponent(entity, type);
	}

	std::shared_ptr<void> Coordinator::Manager::getCopiedEntityComponent(Entity::Type entity, Component::Type type) {
		return component_manager->getCopiedEntityComponent(entity, type);
	}

	void Coordinator::Manager::setEntityComponent(Entity::Type entity, Component::Type type, std::shared_ptr<void> comp) {
		component_manager->setEntityComponent(entity, type, comp);
	}

	size_t Coordinator::Manager::getComponentEntitiesCount(Component::Type comp_type) {
		return component_manager->getComponentEntitiesCount(comp_type);
	}

	std::set<Entity::Type> Coordinator::Manager::getAllComponentEntities(Component::Type comp_type) {
		return component_manager->getAllComponentEntities(comp_type);
	}

	std::unordered_map<std::string, std::shared_ptr<void>> Coordinator::Manager::getAllEntityComponents(Entity::Type entity) const {
		return component_manager->getAllEntityComponents(entity);
	}

	std::unordered_map<std::string, std::shared_ptr<void>> Coordinator::Manager::getAllCopiedEntityComponents(Entity::Type entity) const {
		return component_manager->getAllCopiedEntityComponents(entity);
	}

	std::unordered_map<std::string, Component::Type> Coordinator::Manager::getAllComponentTypes() const {
		return component_manager->getAllComponentTypes();
	}

	size_t Coordinator::Manager::getComponentsCount() const {
		return component_manager->getComponentsCount();
	}
}