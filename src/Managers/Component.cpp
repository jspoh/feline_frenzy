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
void Component::Array<T>::addComponent(Entity::Type entity, T&& component) {
	//Check if entity has already been added
	assert(component_array.find(entity) == component_array.end() && "Entity has already been added for this component.");

	//Emplace component and entity
	component_array.emplace(std::piecewise_construct, std::forward_as_tuple(entity), std::forward_as_tuple(std::move(component)));
}

template<typename T>
void Component::Array<T>::removeComponent(Entity::Type entity) {
	//Check if entity is present within components to delete
	assert(component_array.find(entity) != component_array.end() && "Entity not found. Unable to remove.");

	//Remove entitys
	component_array.erase(entity);
}

template<typename T>
T& Component::Array<T>::getComponent(Entity::Type entity) {
	//Check if entity is present within components
	assert(component_array.find(entity) != component_array.end() && "Entity not found. Unable to retrieve component array.");

	//Return component array
	return component_array.at(entity);
}