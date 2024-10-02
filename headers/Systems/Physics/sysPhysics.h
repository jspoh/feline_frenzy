/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef PHYSICS_HPP
#define PHYSICS_HPP


#include "../headers/Systems/sysInput.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/Physics/sysCollision.h"

namespace Physics {

	class Manager : public System::ISystem {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	public:
		Collision::Manager collision_manager; // Current one instance of collision manager

		//Default Constructor
		Manager() = default;

		//Default Destructor
		~Manager() override = default;

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

		//Init
		void init() override;

		// Runtime scaling and rotation
		void runtimeScaleOrRotate(Transform::Runtime_Transform& runtime_comp, Transform::Transform& transform_comp);

		std::string getSysName() override
		{
			return "Physics System";
		}

		//Update
		void update() override;

		//Move Object
		//void move(Transform::Transform& transform);
	};
}

#endif //!INPUT_HPP