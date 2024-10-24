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

 //Components
#include "Components/cAudio.h"

namespace NIKESAURUS {
	namespace Coordinator {
		class NIKESAURUS_API Service {
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

			//Destroy Entity
			void destroyAllEntities();

			//Get entity count
			int getEntitiesCount();

			/*****************************************************************//**
			* Components Methods
			*********************************************************************/
			template<typename T>
			void addEntityComponent(Entity::Type entity, T&& component);

			template<typename T>
			void removeEntityComponent(Entity::Type entity);

			template<typename T>
			T& getEntityComponent(Entity::Type entity);
		};
	}
}

#endif // !S_COORDINATOR_ECS_H

