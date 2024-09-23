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

void System::ISystem::setSignaure(Component::Signature signature) {
	c_id = signature;
}

Component::Signature System::ISystem::getSignature() const {
	return c_id;
}

/*****************************************************************//**
* System Manager
*********************************************************************/

void System::Manager::updateSystems() {

	//Update all systems
	for (auto& system : systems) {
		system->update();
	}
}

