/*****************************************************************//**
 * \file   Component.cpp
 * \brief  Component manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Component.h"

template<typename T>
void Component::Map<T>::addComponent(Entity::Type entity, T&& component) {
	component_map.emplace(std::piecewise_construct, std::forward_as_tuple(entity), std::forward_as_tuple(std::move(component)));
}

template<typename T>
void Component::Map<T>::removeComponent(Entity::Type entity) {
	component_map.erase(entity);
}

template<typename T>
T& Component::Map<T>::getComponent(Entity::Type entity) {
	return component_map.at(entity);
}