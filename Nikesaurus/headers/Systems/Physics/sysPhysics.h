/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

//#include "Systems/Physics/sysCollision.h"
#include "Managers/ECS/mSystem.h"

namespace NIKESAURUS {
	namespace Physics {

		class Manager : public System::ISystem {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

		public:
			//Collision::Manager collision_manager; // Current one instance of collision manager // Added by MKK

			//Default Constructor
			Manager() = default;

			//Default Destructor
			~Manager() override = default;

			//Init
			void init() override;

			//System name
			std::string getSysName() override
			{
				return "Physics System";
			}

			//Update
			void update() override;
		};
	}
}

#endif //!INPUT_HPP