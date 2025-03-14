/*****************************************************************//**
 * \file   mSystem.cpp
 * \brief  System manager for ECS architecture
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (90%)
 * \co-author Bryan Lim, 2301214, bryanlicheng.l@digipen.edu (10%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/ECS/mSystem.h"
#include "Core/Engine.h"

/*****************************************************************//**
* ISystem
*********************************************************************/
namespace NIKE {

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
		//Set signature
		system_signature.set(component, true);
	}

	void System::ISystem::removeComponentType(Component::Type component) {

		//Set signature
		system_signature.set(component, false);
	}

	bool System::ISystem::checkComponentType(Component::Type component) const {

		//Return if component is present
		return system_signature.test(component);
	}

	Component::Signature System::ISystem::getSignature() const {
		return system_signature;
	}

	void System::ISystem::addEntity(Entity::Type entity) {

		//No exception needed, set returns 0 if duplicate entity is found
		entities.insert(entity);
	}

	void System::ISystem::removeEntity(Entity::Type entity) {
		if (entities.find(entity) == entities.end()) {
			return;
		}

		// remove entity children
		// auto relation = NIKE_METADATA_SERVICE->getEntityRelation(entity);
		// auto* parent = std::get_if<MetaData::Parent>(&relation);

		// if (parent) {
		// 	for (const auto child : parent->childrens) {
		// 		auto e = NIKE_METADATA_SERVICE->getEntityByName(child);

		// 		if (!e.has_value()) {
		// 			continue;
		// 		}

		// 		Entity::Type entity = e.value();
		// 		removeEntity(entity);
		// 	}
		// }

		//No exception needed, set returns 0 if entity is not found
		entities.erase(entity);
	}

	bool System::ISystem::checkEntity(Entity::Type entity) const {
		return entities.find(entity) != entities.end();
	}

	/*****************************************************************//**
	* System Manager
	*********************************************************************/

	void System::Manager::updateEntitiesList(Entity::Type entity, Component::Signature e_signature) {
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
				//Check if there are still components matching
				if ((system->getSignature() & e_signature).any()) {
					system->addEntity(entity);
				}
				else {
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
			if (system->checkEntity(entity))
				system->removeEntity(entity);
		}
	}

	void System::Manager::updateSystems()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
		}

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
			else {
				//Push system no run time
				system_times.push_back(0.0);
			}
		}

		// Call runtime percentage function every 30 seconds
		std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
		double time_since_last_call = std::chrono::duration<double, std::milli>(current_time - last_call_time).count();

		// Every 4 seconds, call function
		// 4000 miliseconds = 4 seconds
		if (time_since_last_call >= 1000)
		{
			// Call to calculate and display system runtime percentage
			NIKE_DEBUG_SERVICE->updateSystemPercentage(system_times, systems);

			// Update the last debug call time to the current time
			last_call_time = current_time;
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}
	}

	std::vector<std::shared_ptr<System::ISystem>>& System::Manager::getAllSystems() {
		return systems;
	}
}