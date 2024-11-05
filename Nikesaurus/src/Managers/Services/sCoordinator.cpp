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

namespace NIKE {
	/*****************************************************************//**
	* Entity Methods
	*********************************************************************/
	Entity::Type Coordinator::Service::createEntity(unsigned int layer_id) {
		return NIKE_ECS_MANAGER->createEntity(layer_id);
	}

	Entity::Type Coordinator::Service::cloneEntity(Entity::Type copy) {
		return NIKE_ECS_MANAGER->cloneEntity(copy);
	}

	void Coordinator::Service::destroyEntity(Entity::Type entity) {
		NIKE_ECS_MANAGER->destroyEntity(entity);
	}

	int Coordinator::Service::getEntityComponentCount(Entity::Type entity) const {
		return NIKE_ECS_MANAGER->getEntityComponentCount(entity);
	}

	void Coordinator::Service::destroyAllEntities() {
		NIKE_ECS_MANAGER->destroyAllEntities();
	}

	int Coordinator::Service::getEntitiesCount() {
		return NIKE_ECS_MANAGER->getEntitiesCount();
	}

	/*****************************************************************//**
	* Component Methods
	*********************************************************************/
	template<typename T>
	void Coordinator::Service::addEntityComponent(Entity::Type entity, T&& component) {
		NIKE_ECS_MANAGER->addEntityComponent(entity, std::move(component));
	}

	void Coordinator::Service::addDefEntityComponent(Entity::Type entity, Component::Type type) {
		NIKE_ECS_MANAGER->addDefEntityComponent(entity, type);
	}

	template<typename T>
	void Coordinator::Service::removeEntityComponent(Entity::Type entity) {
		NIKE_ECS_MANAGER->removeEntityComponent<T>(entity);
	}

	template<typename T>
	T& Coordinator::Service::getEntityComponent(Entity::Type entity) {
		return NIKE_ECS_MANAGER->getEntityComponent<T>(entity);
	}

	template<typename T>
	Component::Type Coordinator::Service::getComponentType() {
		return  NIKE_ECS_MANAGER->getComponentType<T>();
	}

	std::unordered_map<std::string, Component::Type> Coordinator::Service::getAllComponentTypes() const {
		return NIKE_ECS_MANAGER->getAllComponentTypes();
	}

	/*****************************************************************//**
	* Explicit Template Instantiations For Template Functions
	*********************************************************************/
	#define ECS_DEFINE_TYPES(type)    \
	template NIKE_API void Coordinator::Service::addEntityComponent<type>(Entity::Type entity, type&& component); \
	template NIKE_API void Coordinator::Service::removeEntityComponent<type>(Entity::Type entity); \
	template NIKE_API type& Coordinator::Service::getEntityComponent<type>(Entity::Type entity); \
	template NIKE_API Component::Type Coordinator::Service::getComponentType<type>();

	//Define Audio Types
	ECS_DEFINE_TYPES(Audio::SFX)

	//Define Transform Types
	ECS_DEFINE_TYPES(Transform::Transform)

	//Define Physics Types
	ECS_DEFINE_TYPES(Physics::Dynamics)
	ECS_DEFINE_TYPES(Physics::Collider)

	//Define Render Types
	ECS_DEFINE_TYPES(Render::Cam)
	ECS_DEFINE_TYPES(Render::Shape)
	ECS_DEFINE_TYPES(Render::Text)
	ECS_DEFINE_TYPES(Render::Texture)

	//Define animation types
	ECS_DEFINE_TYPES(Animation::Base)
	ECS_DEFINE_TYPES(Animation::Sprite)
}