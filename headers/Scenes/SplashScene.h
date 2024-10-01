/*****************************************************************//**
 * \file   SplashScene.h
 * \brief
 *
 * \author jings
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef SPLASH_SCENE_HPP
#define SPLASH_SCENE_HPP

#include "../headers/Systems/sysScene.h"
#include "../headers/Core/Engine.h"

namespace Splash {
	class Scene : public Scenes::IScene {
	private:
		//Create scene specific variables here
		Entity::Type input; 
		// BGM goes here
		Entity::Type bg_music;

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
