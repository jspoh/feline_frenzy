/*****************************************************************//**
 * \file   Component.cpp
 * \brief  Component manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mComponent.h"

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