/*****************************************************************//**
 * \file   sCoordinator.h
 * \brief  ECS Coordinator for client side usage
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef S_COORDINATOR_ECS_H
#define S_COORDINATOR_ECS_H

 //Include all components here
#include "Components/cAudio.h"
#include "Components/cRender.h"
#include "Components/cTransform.h"
#include "Components/cPhysics.h"
#include "Components/cAnimation.h"

namespace NIKE {
	namespace Coordinator {
		class NIKE_API Service {
		private:
		public:
			//Default constructor
			Service() = default;

			/*****************************************************************//**
			* Entity Methods
			*********************************************************************/
			//Create Entity
			Entity::Type createEntity();

			//Clone entity ( ID of clone returned )
			Entity::Type cloneEntity(Entity::Type copy);

			//Destroy Entity
			void destroyEntity(Entity::Type entity);

			//Get entity component count
			int getEntityComponentCount(Entity::Type entity) const;

			//Destroy Entity
			void destroyAllEntities();

			//Get total entities count
			int getEntitiesCount();

			/*****************************************************************//**
			* Components Methods
			*********************************************************************/
			template<typename T>
			void addEntityComponent(Entity::Type entity, T&& component);

			void addDefEntityComponent(Entity::Type entity, Component::Type type);

			template<typename T>
			void removeEntityComponent(Entity::Type entity);

			template<typename T>
			T& getEntityComponent(Entity::Type entity);

			template<typename T>
			Component::Type getComponentType();

			std::unordered_map<std::string, Component::Type> getAllComponentTypes() const;
		};
	}
}

#endif // !S_COORDINATOR_ECS_H
