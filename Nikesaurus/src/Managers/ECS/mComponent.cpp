/*****************************************************************//**
 * \file   mComponent.cpp
 * \brief  Component manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/ECS/mComponent.h"

namespace NIKE {
	/*****************************************************************//**
	* Component Service
	*********************************************************************/
	void Component::Manager::addDefEntityComponent(Entity::Type entity, Component::Type type) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Adding new default component for entity failed.");
		}

		component_arrays.at(component_string)->createDefEntityComponent(entity);
	}

	void Component::Manager::removeEntityComponent(Entity::Type entity, Component::Type type) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Adding new default component for entity failed.");
		}

		component_arrays.at(component_string)->removeComponent(entity);
	}

	std::shared_ptr<void> Component::Manager::getEntityComponent(Entity::Type entity, Component::Type type) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Fetching component of entity failed.");
		}

		return component_arrays.at(component_string)->getEntityComponent(entity);
	}

	std::shared_ptr<void> Component::Manager::getCopiedEntityComponent(Entity::Type entity, Component::Type type) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Fetching component of entity failed.");
		}

		return component_arrays.at(component_string)->getCopiedEntityComponent(entity);
	}

	void Component::Manager::setEntityComponent(Entity::Type entity, Component::Type type, std::shared_ptr<void> comp) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Fetching component of entity failed.");
		}

		component_arrays[component_string]->setEntityComponent(entity, comp);
	}

	Component::Type Component::Manager::getComponentType(std::string const& type) {
		//Check if component has been registered
		if (component_types.find(type) == component_types.end()) {
			throw std::runtime_error("Component not yet registered or wrong type string.");
		}

		//Return component type
		return component_types.at(type);
	}

	size_t Component::Manager::getComponentEntitiesCount(Component::Type type) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Fetching component of entity failed.");
		}

		return component_arrays.at(component_string)->getComponentEntitiesCount();
	}

	std::set<Entity::Type> Component::Manager::getAllComponentEntities(Component::Type type) {
		std::string component_string;
		for (auto const& comp_type : component_types) {
			if (comp_type.second == type) {
				component_string = comp_type.first;
				break;
			}
		}

		if (component_string == "") {
			throw std::runtime_error("Component not registered. Fetching component of entity failed.");
		}

		return component_arrays.at(component_string)->getComponentEntities();
	}

	void Component::Manager::cloneEntity(Entity::Type clone, Entity::Type copy) {
		for (auto& c_array : component_arrays) {
			c_array.second->cloneEntity(clone, copy);
		}
	}

	void Component::Manager::entityDestroyed(Entity::Type entity) {
		for (auto& c_array : component_arrays) {
			c_array.second->entityDestroyed(entity);
		}
	}

	std::unordered_map<std::string, std::shared_ptr<void>> Component::Manager::getAllEntityComponents(Entity::Type entity) const {
		std::unordered_map<std::string, std::shared_ptr<void>> comp_map;

		//Get all comp strings
		for (auto const& comp : component_arrays) {
			if (comp.second->checkEntity(entity)) {
				comp_map.emplace(comp.first, comp.second->getEntityComponent(entity));
			}
		}

		return comp_map;
	}

	std::unordered_map<std::string, std::shared_ptr<void>> Component::Manager::getAllCopiedEntityComponents(Entity::Type entity) const {
		std::unordered_map<std::string, std::shared_ptr<void>> comp_map;

		//Get all comp strings
		for (auto const& comp : component_arrays) {
			if (comp.second->checkEntity(entity)) {
				comp_map.emplace(comp.first, comp.second->getCopiedEntityComponent(entity));
			}
		}

		return comp_map;
	}

	std::unordered_map<std::string, Component::Type> Component::Manager::getAllComponentTypes() const {
		return component_types;
	}

	size_t Component::Manager::getComponentsCount() const {
		return component_types.size();
	}
}