/*****************************************************************//**
 * \file   sCoordinator.cpp
 * \brief  ECS Coordinator for client side usage
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/sCoordinator.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* Entity Methods
	*********************************************************************/
	Entity::Type Coordinator::Service::createEntity() {
		return NIKEEngine.getService<Coordinator::Manager>()->createEntity();
	}

	Entity::Type Coordinator::Service::cloneEntity(Entity::Type copy) {
		return NIKEEngine.getService<Coordinator::Manager>()->cloneEntity(copy);
	}

	void Coordinator::Service::destroyEntity(Entity::Type entity) {
		NIKEEngine.getService<Coordinator::Manager>()->destroyEntity(entity);
	}

	void Coordinator::Service::destroyAllEntities() {
		NIKEEngine.getService<Coordinator::Manager>()->destroyAllEntities();
	}

	int Coordinator::Service::getEntitiesCount() {
		return NIKEEngine.getService<Coordinator::Manager>()->getEntitiesCount();
	}

	/*****************************************************************//**
	* Component Methods
	*********************************************************************/
	template<typename T>
	void Coordinator::Service::addEntityComponent(Entity::Type entity, T&& component) {
		NIKEEngine.getService<Coordinator::Manager>()->addEntityComponent(entity, std::move(component));
	}

	template<typename T>
	void Coordinator::Service::removeEntityComponent(Entity::Type entity) {
		NIKEEngine.getService<Coordinator::Manager>()->removeEntityComponent<T>(entity);
	}

	template<typename T>
	T& Coordinator::Service::getEntityComponent(Entity::Type entity) {
		return NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<T>(entity);
	}

	/*****************************************************************//**
	* Explicit Template Instantiations For Template Functions
	*********************************************************************/
	#define COMPONENT_TYPES \
		X(Audio::SFX) \
		X(Render::Shape) \
		X(Transform::Transform)

	#define X(type) template NIKESAURUS_API void Coordinator::Service::addEntityComponent<type>(Entity::Type entity, type&& component);
	COMPONENT_TYPES
	#undef X

	#define X(type) template NIKESAURUS_API void Coordinator::Service::removeEntityComponent<type>(Entity::Type entity);
	COMPONENT_TYPES
	#undef X

	#define X(type) template NIKESAURUS_API type& Coordinator::Service::getEntityComponent<type>(Entity::Type entity);
	COMPONENT_TYPES
	#undef X

	#undef COMPONENT_TYPES
}