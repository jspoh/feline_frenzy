/*****************************************************************//**
 * \file   System.cpp
 * \brief  System manager for ecs architecture
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "System.h"

/*****************************************************************//**
* ISystem
*********************************************************************/

void System::ISystem::setActiveState(bool state) {
	b_system_active = state;
}

bool System::ISystem::getActiveState() const {
	return b_system_active;
}

void System::ISystem::setSignature(Component::Signature signature) {
	c_id = signature;
}

Component::Signature System::ISystem::getSignature() const {
	return c_id;
}

void System::ISystem::addEntity(Entity::Type entity) {
	entities.insert(entity);
}

void System::ISystem::removeEntity(Entity::Type entity) {
	entities.erase(entity);
}

/*****************************************************************//**
* System Manager
*********************************************************************/

void System::Manager::updateEntitiesList(Entity::Type entity, Component::Signature e_signature) {
	for (auto& system : systems) {
		if ((system->getSignature() & e_signature) == system->getSignature()) {
			system->addEntity(entity);
		}
		else {
			system->removeEntity(entity);
		}
	}
}

void System::Manager::entityDestroyed(Entity::Type entity) {

	//Remove entity from all systems
	for (auto& system : systems) {
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
