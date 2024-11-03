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

	std::unordered_map<std::string, Component::Type> Component::Manager::getAllComponentTypes() const {
		return component_types;
	}
}