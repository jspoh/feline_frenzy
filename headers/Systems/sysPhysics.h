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
#include "../headers/Systems/Render/Object.h"

namespace Physics {

	class Manager : public System::ISystem {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	public:
		//Default Constructor
		Manager() = default;

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

		//Init
		void init() override;

		//Update
		void update() override;

		//Move Object
		void move(Object* object);

		//Default Destructor
		~Manager() override = default;
	};
}

#endif //!INPUT_HPP