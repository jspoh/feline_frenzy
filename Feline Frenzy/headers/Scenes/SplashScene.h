///*****************************************************************//**
// * \file   SplashScene.h
// * \brief
// * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (90%)
// * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (10%)
// * \date   September 2024
// * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
// *********************************************************************/
//
#pragma once

#ifndef SPLASH_SCENE_HPP
#define SPLASH_SCENE_HPP

namespace Splash {
	class Scene : public NIKE::Scenes::IScene {
	private:

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
