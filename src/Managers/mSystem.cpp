/*****************************************************************//**
 * \file   System.cpp
 * \brief  System manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mSystem.h"

/*****************************************************************//**
* ISystem
*********************************************************************/

void System::ISystem::setComponentsLinked(bool state) {
	b_components_linked = state;
}

bool System::ISystem::getComponentsLinked() const {
	return b_components_linked;
}

void System::ISystem::setActiveState(bool state) {
	b_system_active = state;
}

bool System::ISystem::getActiveState() const {
	return b_system_active;
}

void System::ISystem::addComponentType(Component::Type component) {
	//Insert component types
	component_types.insert(component);

	//Set signature
	system_signature.set(component, true);
}

void System::ISystem::removeComponentType(Component::Type component) {
	//Insert component types
	component_types.erase(component);

	//Set signature
	system_signature.set(component, false);
}

bool System::ISystem::checkComponentType(Component::Type component) const {

	//Return if component is present
	return component_types.find(component) != component_types.end();
}

Component::Signature System::ISystem::getSignature() const {
	return system_signature;
}

void System::ISystem::addEntity(Entity::Type entity) {

	//No exception needed, set returns 0 if duplicate entity is found
	entities.insert(entity);
}

void System::ISystem::removeEntity(Entity::Type entity) {

	//No exception needed, set returns 0 if entity is not found
	entities.erase(entity);
}

bool System::ISystem::checkEntity(Entity::Type entity) const {
	return entities.find(entity) != entities.end();
}

/*****************************************************************//**
* System Manager
*********************************************************************/

void System::Manager::updateEntitiesList(Entity::Type entity, Component::Signature e_signature, Component::Type component, bool b_component_added) {
	for (auto& system : systems) {
		//Check if systems components are set to linked
		if (system->getComponentsLinked()) {
			//Check signature
			if ((system->getSignature() & e_signature) == system->getSignature()) {
				system->addEntity(entity);
			}
			else {
				system->removeEntity(entity);
			}
		}
		else {
			//Check component added
			if (system->checkComponentType(component) && b_component_added) {
				system->addEntity(entity);
			}

			//Check component removed
			if (system->checkComponentType(component) && !b_component_added) {
				system->removeEntity(entity);
			}
		}
	}
}

void System::Manager::entityDestroyed(Entity::Type entity) {

	//Remove entity from all systems
	for (auto& system : systems) {
		if(system->checkEntity(entity))
		system->removeEntity(entity);
	}
}

void System::Manager::updateSystems() {

	//Update all systems
	for (auto& system : systems) {
		if (system->getActiveState())
		system->update();
	}
}
