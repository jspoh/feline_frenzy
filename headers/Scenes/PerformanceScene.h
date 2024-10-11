/*****************************************************************//**
 * \file   PerformanceScene.h
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef PERFORMANCE_SCENE_HPP
#define PERFORMANCE_SCENE_HPP

#include "../headers/Systems/sysScene.h"

namespace Performance {
	class Scene : public Scenes::IScene {
	private:

		// Easy reference to entities types
		std::unordered_map<std::string, Entity::Type> entities;
	public:

		//Dfeault constructor
		Scene() = default;

		//Scene phases
		void load() override;
		void init() override;
		void exit() override;
		void unload() override;


		//Default destructor
		~Scene() override = default;
	};
}

#endif // !PERFORMANCE_SCENE_HPP