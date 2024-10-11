/*****************************************************************//**
 * \file   sysAnimationController.h
 * \brief  animation controller system
 * 
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef OBJECT_SPAWNER_HPP
#define OBJECT_SPAWNER_HPP

#include "../headers/Managers/mSystem.h"

namespace ObjectSpawner {

	//Spawn object
	struct Spawn {
		int count;
		int mouse_type;
		int key_type;

		Spawn(int spawn_count, int mouse_type, int key_type)
			: count{ spawn_count }, mouse_type { mouse_type }, key_type { key_type } {}
	};

	//Spawn Manager
	class Manager : public System::ISystem {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Generate random
		void generateRandomObject();
	public:
		//Default constructor
		Manager() = default;

		//Init Inputs
		void init() override;

		std::string getSysName() override
		{
			return "Object Spawner System";
		}

		//Update Inputs
		bool update() override;

		//Default Destructor
		~Manager() override = default;
	};
}

#endif //!OBJECT_SPAWNER_HPP
