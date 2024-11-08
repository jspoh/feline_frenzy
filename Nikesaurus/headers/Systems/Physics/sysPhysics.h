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

#include "Systems/Physics/sysCollision.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"
#include "Systems/GameLogic/sysLua.h"

namespace NIKE {
	namespace Physics {

		class Manager : 
			public System::ISystem,
			public Lua::ILuaBind,
			public Events::IEventListener<Physics::ChangePhysicsEvent> {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Collision sub system
			std::unique_ptr<Collision::System> collision_system;

			//Apply forces
			void applyXForce(Entity::Type entity, float force);
			void applyYForce(Entity::Type entity, float force);
		public:
			//Default Constructor
			Manager() = default;

			//Default Destructor
			~Manager() = default;

			//Init
			void init() override;

			//System name
			std::string getSysName() override
			{
				return "Physics System";
			}

			//Update
			void update() override;

			//Register lua bindings
			void registerLuaBindings(sol::state& lua_state) override;

			//On change physics event
			void onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) override;
		};
	}
}

#endif //!INPUT_HPP