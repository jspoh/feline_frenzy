/*****************************************************************//**
 * \file   mComponent.cpp
 * \brief  Component manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/ECS/mComponent.h"

namespace NIKESAURUS {
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
}