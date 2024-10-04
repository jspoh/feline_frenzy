/*****************************************************************//**
 * \file   SplashScene.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SPLASH_SCENE_HPP
#define SPLASH_SCENE_HPP

#include "../headers/Systems/sysScene.h"
#include "../headers/Core/Engine.h"

namespace Splash {
	class Scene : public Scenes::IScene {
	private:
		std::unordered_map<std::string, Entity::Type> entities;

		//Register components
		void registerComponents();

		//Register static systems
		void registerStaticSystems();

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

#endif // !SPLASH_SCENE_HPP
