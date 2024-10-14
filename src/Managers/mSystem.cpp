/*****************************************************************//**
 * \file   mSystem.cpp
 * \brief  System manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (90%)
 * \co-author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (10%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mSystem.h"
#include "../headers/Core/Engine.h"

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

void System::Manager::cloneEntity(Entity::Type clone, Entity::Type copy) {
	//Remove entity from all systems
	for (auto& system : systems) {
		if (system->checkEntity(copy))
			system->addEntity(clone);
	}
}

void System::Manager::entityDestroyed(Entity::Type entity) {

	//Remove entity from all systems
	for (auto& system : systems) {
		if(system->checkEntity(entity))
		system->removeEntity(entity);
	}
}

void System::Manager::updateSystems() 
{
	std::chrono::steady_clock::time_point total_start_time = std::chrono::steady_clock::now();
	// Vector to hold each of the system duration
	std::vector<double> system_times;

	// Update all systems
	for (auto const& system : systems) {
		if (system->getActiveState())
		{
			std::chrono::steady_clock::time_point system_start_time = std::chrono::steady_clock::now();

			//Break system update loop if system update returns true
			system->update();

			std::chrono::steady_clock::time_point system_end_time = std::chrono::steady_clock::now();

			// Calculate the time taken by the system
			std::chrono::duration<double, std::milli> system_duration = system_end_time - system_start_time;
			system_times.push_back(system_duration.count());
		}
	}
	// Track total end time for the update call
	std::chrono::steady_clock::time_point total_end_time = std::chrono::steady_clock::now();
	double total_game_loop_time = std::chrono::duration<double, std::milli>(total_end_time - total_start_time).count();

	// Call runtime percentage function every 30 seconds
	std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
	double time_since_last_call = std::chrono::duration<double, std::milli>(current_time - last_call_time).count();

	// Every 4 seconds, call function
	// 4000 miliseconds = 4 seconds
	if (time_since_last_call >= 4000)
	{
		// Call to calculate and display system runtime percentage
		NIKEEngine.accessDebug()->systemRuntimePercentage(total_game_loop_time, system_times, systems);

		// Update the last debug call time to the current time
		last_call_time = current_time;
	}
}
